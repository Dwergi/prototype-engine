//
// WaterSystem.h
// Copyright (C) Sebastian Nordgren 
// December 3rd 2018
//

#pragma once

#include "IDebugPanel.h"
#include "Material.h"
#include "Shader.h"
#include "System.h"

namespace dd
{
	struct TerrainParameters;

	struct WaterSystem : ddc::System, IDebugPanel
	{
		WaterSystem( const TerrainParameters& params );

		virtual void Update( const ddc::UpdateData& update_data ) override;
		virtual void Initialize( ddc::World& world ) override;

	private:

		static ddr::ShaderHandle s_shader;
		static ddr::MaterialHandle s_material;

		int m_waterChunks { 0 };

		const TerrainParameters& m_terrainParams;
		float m_waterHeight { 32.0f };

		bool m_regenerate { false };

		virtual const char* GetDebugTitle() const override { return "Water"; }
		virtual void DrawDebugInternal( ddc::World& world ) override;

		ddc::Entity CreateWaterEntity( ddc::World& world, glm::vec2 chunk_pos ) const;
	};
}