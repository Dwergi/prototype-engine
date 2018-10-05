//
// TerrainRenderer.cpp
// Copyright (C) Sebastian Nordgren 
// September 30th 2018
//

#include "PCH.h"
#include "TerrainRenderer.h"

#include "Mesh.h"
#include "MeshComponent.h"
#include "TerrainChunk.h"
#include "TerrainChunkComponent.h"
#include "TerrainParameters.h"

namespace ddr
{
	struct Wireframe
	{
		bool Enabled { false };

		glm::vec3 Colour { 0, 1.0f, 0 };
		float Width { 2.0f };

		glm::vec3 EdgeColour { 0, 0, 0 };
		float EdgeWidth { 0.5f };

		float MaxDistance { 250.0f };

		void UpdateUniforms( ddr::UniformStorage& uniforms ) const
		{
			uniforms.Set( "Wireframe.Enabled", Enabled );
			uniforms.Set( "Wireframe.Colour", Colour );
			uniforms.Set( "Wireframe.Width", Width );
			uniforms.Set( "Wireframe.EdgeColour", EdgeColour );
			uniforms.Set( "Wireframe.EdgeWidth", EdgeWidth );
			uniforms.Set( "Wireframe.MaxDistance", MaxDistance );
		}
	};

	TerrainRenderer::TerrainRenderer( const dd::TerrainParameters& params ) : 
		Renderer( "Terrain" ),
		m_params( params )
	{
		Require<dd::MeshComponent>();
		Require<dd::TerrainChunkComponent>();

		m_wireframe = new Wireframe();
	}

	void TerrainRenderer::RenderInit( ddc::World& world )
	{
		dd::TerrainChunk::CreateRenderResources();
	}

	void TerrainRenderer::Render( const ddr::RenderData& data )
	{
		ddr::UniformStorage& uniforms = data.Uniforms();

		m_params.UpdateUniforms( uniforms );

		m_wireframe->UpdateUniforms( uniforms );

		auto meshes = data.Get<dd::MeshComponent>();
		auto chunks = data.Get<dd::TerrainChunkComponent>();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			chunks[i].Chunk->RenderUpdate( data.Uniforms() );
		}
	}

	void TerrainRenderer::DrawDebugInternal( ddc::World& world )
	{
		if( ImGui::TreeNodeEx( "Wireframe", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::Checkbox( "Enabled", &m_wireframe->Enabled );

			ImGui::DragFloat( "Width", &m_wireframe->Width, 0.01f, 0.0f, 10.0f );

			ImGui::ColorEdit3( "Colour", glm::value_ptr( m_wireframe->Colour ) );

			ImGui::DragFloat( "Edge Width", &m_wireframe->EdgeWidth, 0.01f, 0.0f, m_wireframe->Width );

			ImGui::ColorEdit3( "Edge Colour", glm::value_ptr( m_wireframe->EdgeColour ) );

			ImGui::DragFloat( "Max Distance", &m_wireframe->MaxDistance, 1.0f, 0.0f, 1000.0f );

			ImGui::TreePop();
		}
	}
}