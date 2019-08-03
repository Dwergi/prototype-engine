//
// Luxport.h - Game file for Luxport GMTK Game Jame 2019.
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#include "PCH.h"
#include "LuxportGame.h"

#include "EntityPrototype.h"
#include "IWindow.h"
#include "OrthoCamera.h"
#include "SpriteAnimationComponent.h"
#include "SpriteComponent.h"
#include "SpriteRenderer.h"
#include "Texture.h"
#include "WorldRenderer.h"

#pragma optimize("",off)

namespace lux
{
	static dd::Service<ddr::SpriteManager> s_spriteManager;
	static dd::Service<ddr::TextureManager> s_textureManager;
	static dd::Service<dd::IWindow> s_window;
	static dd::Service<ddr::WorldRenderer> s_renderer;

	static glm::vec2 PixelsToNormalized(glm::ivec2 pos_px)
	{
		glm::vec2 win_size = s_window->GetSize();
		glm::vec2 normalized = pos_px;
		normalized = ((normalized - (win_size / 2.0f)) / win_size) * 2.0f;

		DD_ASSERT(normalized.x >= -1 && normalized.x <= 1 && normalized.y >= -1 && normalized.y <= 1);
		return normalized;
	}

	static void CreateSprite(ddc::World& world, ddr::SpriteHandle sprite_h, glm::ivec2 pos_px, glm::ivec2 size_px)
	{
		ddc::Entity new_entity = world.CreateEntity<dd::SpriteComponent>();
		world.AddTag(new_entity, ddc::Tag::Visible);
		dd::SpriteComponent* sprite_cmp = world.Access<dd::SpriteComponent>(new_entity);
		sprite_cmp->Sprite = sprite_h;
		sprite_cmp->Position = PixelsToNormalized(pos_px);
		sprite_cmp->Size = PixelsToNormalized(size_px);
	}

	void LuxportGame::Initialize(ddc::World& world)
	{
		dd::Services::RegisterInterface<ddr::ICamera>(new ddr::OrthoCamera());

		ddr::SpriteRenderer& sprite_renderer = dd::Services::Register(new ddr::SpriteRenderer());
		s_renderer->Register(sprite_renderer);

		ddr::TextureHandle tileset_h = s_textureManager->Load("spacestation_tileset.png");

		std::vector<ddr::SpriteHandle> sprites = s_spriteManager->LoadSpriteSheet("space_station", tileset_h, glm::ivec2(16, 16));

		const int ScaleFactor = 2;
		const int SizeInPixels = (16 * ScaleFactor);

		glm::ivec2 window_size = s_window->GetSize();
		int sprites_per_row = window_size.x / SizeInPixels;
		int rows = window_size.y / SizeInPixels;

		DD_ASSERT((size_t) sprites_per_row * rows >= sprites.size(), "Window too small!");

		DD_ASSERT(PixelsToNormalized(glm::ivec2(0, 0)) == glm::vec2(-1, -1));
		DD_ASSERT(PixelsToNormalized(window_size / 2) == glm::vec2(0, 0));
		DD_ASSERT(PixelsToNormalized(window_size) == glm::vec2(1, 1));

		for (int i = 0; i < sprites.size(); ++i)
		{
			int y = i / sprites_per_row;
			int x = i - y * sprites_per_row;

			CreateSprite(world, sprites[i], glm::ivec2(x * SizeInPixels, y * SizeInPixels), glm::ivec2(SizeInPixels, SizeInPixels));
		}
	}

	void LuxportGame::Shutdown(ddc::World& world)
	{

	}

	void LuxportGame::Update(ddc::World& world)
	{

	} 

	void LuxportGame::RenderUpdate(ddc::World& world)
	{

	}
}