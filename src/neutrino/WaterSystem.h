//
// WaterSystem.h
// Copyright (C) Sebastian Nordgren 
// December 3rd 2018
//

#pragma once

#include "IDebugPanel.h"
#include "JobSystem.h"
#include "Noise.h"

#include "ddc/System.h"

#include "ddr/Material.h"

namespace neut
{
	struct TerrainParameters;
	struct WaterComponent;

	struct WaterSystem : ddc::System, dd::IDebugPanel
	{
		WaterSystem(const TerrainParameters& params);

		virtual void Update(ddc::UpdateData& update_data) override;
		virtual void Initialize(ddc::EntityLayer& entities) override;

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
		virtual void DrawDebugInternal() override;

		void CreateWaterEntity(ddc::UpdateData& update_data, glm::vec2 chunk_pos) const;
	};
}