//
// TerrainRenderer.cpp
// Copyright (C) Sebastian Nordgren 
// September 30th 2018
//

#include "PCH.h"
#include "neutrino/TerrainRenderer.h"

#include "MeshComponent.h"

#include "ddr/Mesh.h"

#include "neutrino/TerrainChunk.h"
#include "neutrino/TerrainChunkComponent.h"
#include "neutrino/TerrainParameters.h"

namespace neut
{
	struct Wireframe
	{
		bool Enabled { true };

		glm::vec3 Colour { 0, 1.0f, 0 };
		float Width { 2.0f };

		glm::vec3 EdgeColour { 0, 0, 0 };
		float EdgeWidth { 0.5f };

		float MaxDistance { 250.0f };

		void UpdateUniforms(ddr::UniformStorage& uniforms) const
		{
			uniforms.Set("Wireframe.Enabled", Enabled);
			uniforms.Set("Wireframe.Colour", Colour);
			uniforms.Set("Wireframe.Width", Width);
			uniforms.Set("Wireframe.EdgeColour", EdgeColour);
			uniforms.Set("Wireframe.EdgeWidth", EdgeWidth);
			uniforms.Set("Wireframe.MaxDistance", MaxDistance);
		}
	};

	TerrainRenderer::TerrainRenderer(const neut::TerrainParameters& params) :
		IRenderer("Terrain"),
		m_params(params)
	{
		m_wireframe = new Wireframe();
	}

	void TerrainRenderer::Initialize()
	{
		
	}

	void TerrainRenderer::Update(ddr::RenderData& data)
	{
		data.Layer().ForAllWith<neut::TerrainChunkComponent>(
			[this](ddc::Entity, neut::TerrainChunkComponent& chunk)
			{
				chunk.Chunk->RenderUpdate();
			});
	}

	void TerrainRenderer::Render(const ddr::RenderData& data)
	{
		m_params.UpdateUniforms(data.Uniforms());
		m_wireframe->UpdateUniforms(data.Uniforms());
	}

	void TerrainRenderer::DrawDebugInternal()
	{
		ImGui::Checkbox("Enabled", &m_wireframe->Enabled);

		ImGui::DragFloat("Width", &m_wireframe->Width, 0.01f, 0.0f, 10.0f);

		ImGui::ColorEdit3("Colour", glm::value_ptr(m_wireframe->Colour));

		ImGui::DragFloat("Edge Width", &m_wireframe->EdgeWidth, 0.01f, 0.0f, m_wireframe->Width);

		ImGui::ColorEdit3("Edge Colour", glm::value_ptr(m_wireframe->EdgeColour));

		ImGui::DragFloat("Max Distance", &m_wireframe->MaxDistance, 1.0f, 0.0f, 1000.0f);
	}
}