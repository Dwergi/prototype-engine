#pragma once

#include "Entity.h"

namespace ddc
{
	struct World;
}

namespace lux
{
	struct LuxportMap
	{
		LuxportMap(int index);

		void Load(ddc::World& world);
		void Unload(ddc::World& world);

		int GetIndex() const { return m_index; }

		ddc::Entity GetStart() const { return m_start; }
		ddc::Entity GetEnd() const { return m_end; }

	private:

		int m_index { 0 };
		std::string m_folder;
		std::vector<ddc::Entity> m_entities;
		ddc::Entity m_start;
		ddc::Entity m_end;

		void LoadLayer(ddc::World& world, int layer);
		void HandleSpecialTiles(ddc::World& world, ddc::Entity entity, std::string tileset, int tile_index);
	};
}