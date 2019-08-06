#include "PCH.h"
#include "LuxportMap.h"

#include "d2d/BoxPhysicsComponent.h"
#include "d2d/CirclePhysicsComponent.h"
#include "File.h"
#include "lux/LuxLightComponent.h"
#include "SpriteSheet.h"
#include "d2d/SpriteComponent.h" 
#include "d2d/SpriteTileComponent.h"

#include <fmt/format.h>

// TODO: Move somewhere else, utils?
// trim from end (in place)
static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

namespace lux
{
	namespace SpecialTiles
	{
		static const int RedLights[] = { 48, 49 };
		static const int YellowLights[] = { 50, 51 };
		static const int Solids[] = { 0, 1, 2, 3, 4, 5, 11, 13, 14, 15, 22, 23, 24 };

		static const char* const SolidTileSet = "spacestation_tileset.png";
		static const char* const InteractionTileSet = "spacestation_objects.png";

		static const int End = 55;
		static const int Start = 83;

		template <size_t Size>
		static bool IsOneOf(int tile_idx, const int (&array)[Size])
		{
			for (size_t i = 0; i < Size; ++i)
			{
				if (array[i] == tile_idx)
				{
					return true;
				}
			}
			return false;
		}
	}

	static dd::Service<ddr::SpriteSheetManager> s_spriteSheetManager;

	static ddc::Entity CreateSpriteEntity(ddc::EntitySpace& entities, ddr::SpriteHandle sprite_h, glm::ivec2 coord, int z_index)
	{
		ddc::Entity new_entity = entities.CreateEntity<d2d::SpriteComponent, d2d::SpriteTileComponent>();
		entities.AddTag(new_entity, ddc::Tag::Visible);

		d2d::SpriteComponent* sprite_cmp = entities.Access<d2d::SpriteComponent>(new_entity);
		sprite_cmp->Sprite = sprite_h;
		sprite_cmp->ZIndex = z_index;

		d2d::SpriteTileComponent* sprite_tile_cmp = entities.Access<d2d::SpriteTileComponent>(new_entity);
		sprite_tile_cmp->Coordinate = coord;

		return new_entity;
	}

	void LuxportMap::HandleSpecialTiles(ddc::EntitySpace& entities, ddc::Entity entity, std::string tileset, int tile_index)
	{
		const bool solid_tileset = tileset == SpecialTiles::SolidTileSet;

		if (solid_tileset)
		{
			if (SpecialTiles::IsOneOf(tile_index, SpecialTiles::Solids))
			{
				entities.Add<d2d::BoxPhysicsComponent>(entity);
				entities.AddTag(entity, ddc::Tag::Static);
			}
		}

		const bool interaction_tileset = tileset == SpecialTiles::InteractionTileSet;
		if (interaction_tileset)
		{
			if (tile_index == SpecialTiles::Start)
			{
				m_start = entity;
			}

			if (tile_index == SpecialTiles::End)
			{
				lux::LuxLightComponent& light = entities.Add<lux::LuxLightComponent>(entity);
				light.Type = lux::LightType::Exit;
				m_end = entity;
			}

			if (SpecialTiles::IsOneOf(tile_index, SpecialTiles::RedLights))
			{
				lux::LuxLightComponent& light = entities.Add<lux::LuxLightComponent>(entity);
				light.Type = lux::LightType::Red;
			}

			if (SpecialTiles::IsOneOf(tile_index, SpecialTiles::YellowLights))
			{
				lux::LuxLightComponent& light = entities.Add<lux::LuxLightComponent>(entity);
				light.Type = lux::LightType::Yellow;
			}
		}
	}

	LuxportMap::LuxportMap(int index)
	{
		m_index = index;
		m_folder = fmt::format("map{}\\", index);
	}

	void LuxportMap::LoadLayer(ddc::EntitySpace& entities, int layer)
	{
		std::string filename = fmt::format("{}layer_{}.csv", m_folder, layer);
		if (!dd::File::Exists(filename))
		{
			return;
		}

		dd::File map_file(filename);
		std::string contents;
		if (map_file.Read(contents) == 0)
		{
			return;
		}

		size_t first_line = contents.find_first_of("\n");
		std::string tileset_name = contents.substr(0, first_line);
		trim(tileset_name);

		contents = contents.substr(first_line);
		ltrim(contents);

		const ddr::SpriteSheetHandle spritesheet_h = s_spriteSheetManager->Find(tileset_name);
		const ddr::SpriteSheet* spritesheet = spritesheet_h.Get();

		std::string number;
		glm::ivec2 coord(0, 0);

		for (char c : contents)
		{
			if (isalnum(c))
			{
				number += c;
			}
			else if (c == ',' || c == '\t' || c == '\n')
			{
				if (!number.empty())
				{
					int tile_index = atoi(number.c_str());
					ddc::Entity created = CreateSpriteEntity(entities, spritesheet->Get(tile_index), coord, layer);

					HandleSpecialTiles(entities, created, tileset_name, tile_index);

					m_entities.push_back(created);
					number.clear();
				}

				if (c == ',' || c == '\t')
				{
					++coord.x;
				}
				else if (c == '\n')
				{
					coord.x = 0;
					++coord.y;
				}
			}
			// ignore other chars
		}
	}

	void LuxportMap::Load(ddc::EntitySpace& entities)
	{
		const int MAX_LAYERS = 5;

		for (int i = 0; i < MAX_LAYERS; ++i)
		{
			LoadLayer(entities, i);
		}

		DD_ASSERT(m_start.IsValid());
		DD_ASSERT(m_end.IsValid());
	}

	void LuxportMap::Unload(ddc::EntitySpace& entities)
	{
		for (ddc::Entity e : m_entities)
		{
			entities.RemoveTag(e, ddc::Tag::Visible);
			entities.DestroyEntity(e);
		}
		m_entities.clear();
	}
}