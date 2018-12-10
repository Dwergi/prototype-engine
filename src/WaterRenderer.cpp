//
// WaterRenderer.h - Renderer for water.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#include "PCH.h"
#include "WaterRenderer.h"

#include "MeshComponent.h"
#include "MeshUtils.h"
#include "TransformComponent.h"
#include "WaterComponent.h"

namespace ddr
{
	WaterRenderer::WaterRenderer() :
		Renderer( "Lines" )
	{
		Require<dd::WaterComponent>();
		Require<dd::TransformComponent>();
		Require<dd::MeshComponent>();

		m_renderState.BackfaceCulling = true;
		m_renderState.Blending = true;
		m_renderState.Depth = true;
	}

	void WaterRenderer::RenderUpdate( ddc::World& world )
	{
		world.ForAllWith<dd::WaterComponent, dd::MeshComponent>( [&world]( ddc::Entity entity, dd::WaterComponent& water, dd::MeshComponent& mesh_cmp )
		{
			if( water.Dirty && mesh_cmp.Mesh.IsValid() )
			{
				ddr::Mesh* mesh = mesh_cmp.Mesh.Access();
				mesh->SetPositions( dd::ConstBuffer<glm::vec3>( water.Vertices ) );
				mesh->SetIndices( dd::ConstBuffer<uint>( dd::GetGridIndices( dd::WaterComponent::VertexCount, water.LOD ) ) );

				water.Dirty = false;
			}
		} );
	}

	void WaterRenderer::Render( const RenderData& render_data )
	{
		auto waters = render_data.Get<dd::WaterComponent>();
		auto transforms = render_data.Get<dd::TransformComponent>();
		auto meshes = render_data.Get<dd::MeshComponent>();

		for( size_t i = 0; i < render_data.Size(); ++i )
		{
			
		}
	}
}
