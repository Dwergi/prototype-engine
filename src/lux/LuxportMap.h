//
// LuxportMap.h
// Copyright (C) Sebastian Nordgren 
// August 4th 2019
//

#pragma once

namespace lux
{
	struct LuxportMap
	{
		LuxportMap(int index);

		void Load(ddc::EntityLayer& entities);
		void Unload(ddc::EntityLayer& entities);

		int GetIndex() const { return m_index; }

		ddc::Entity GetStart() const { return m_start; }
		ddc::Entity GetEnd() const { return m_end; }

	private:

		int m_index { 0 };
		std::string m_folder;
		std::vector<ddc::Entity> m_entities;
		ddc::Entity m_start;
		ddc::Entity m_end;

		void LoadLayer(ddc::EntityLayer& entities, int layer);
		void HandleSpecialTiles(ddc::EntityLayer& entities, ddc::Entity entity, std::string tileset, int tile_index);
	};
}