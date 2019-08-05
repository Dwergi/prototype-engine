//
// WaterSystem.h
// Copyright (C) Sebastian Nordgren 
// December 3rd 2018
//

#pragma once

#include "IDebugPanel.h"
#include "JobSystem.h"
#include "Material.h"
#include "Noise.h"
#include "System.h"

namespace dd
{
	struct TerrainParameters;
	struct WaterComponent;

	struct WaterSystem : ddc::System, IDebugPanel
	{
		WaterSystem( const TerrainParameters& params );

		virtual void Update( const ddc::UpdateData& update_data ) override;
		virtual void Initialize( ddc::EntitySpace& entities ) override;

	private:

		static ddr::MaterialHandle s_material;
		static ddr::RenderState s_renderState;

		int m_waterChunks { 0 };
		float m_waveHeight { 2.0f };
		glm::vec2 m_waveLateral { 0.25f, 0.25f };

		ddm::NoiseParameters m_noiseParams;

		const TerrainParameters& m_terrainParams;
		float m_waterHeight { 32.0f };

		bool m_regenerate { false };

		virtual const char* GetDebugTitle() const override { return "Water"; }
		virtual void DrawDebugInternal( ddc::EntitySpace& entities ) override;

		ddc::Entity CreateWaterEntity( ddc::EntitySpace& entities, glm::vec2 chunk_pos ) const;
	};
}