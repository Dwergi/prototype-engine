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
#include "Shader.h"
#include "System.h"

namespace dd
{
	struct TerrainParameters;
	struct WaterComponent;

	struct WaterSystem : ddc::System, IDebugPanel
	{
		WaterSystem( const TerrainParameters& params, dd::JobSystem& jobsystem );

		virtual void Update( const ddc::UpdateData& update_data ) override;
		virtual void Initialize( ddc::World& world ) override;

	private:

		dd::JobSystem& m_jobSystem;

		int m_waterChunks { 0 };
		float m_waveHeight { 2.0f };
		glm::vec2 m_waveLateral { 0.25f, 0.25f };

		ddm::NoiseParameters m_noiseParams;

		const TerrainParameters& m_terrainParams;
		float m_waterHeight { 32.0f };

		bool m_regenerate { false };

		virtual const char* GetDebugTitle() const override { return "Water"; }
		virtual void DrawDebugInternal( ddc::World& world ) override;

		ddc::Entity CreateWaterEntity( ddc::World& world, glm::vec2 chunk_pos ) const;
	};
}