//
// WaterRenderer.h - Renderer for water.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#include "PCH.h"
#include "WaterRenderer.h"

#include "BoundBoxComponent.h"
#include "ColourComponent.h"
#include "JobSystem.h"
#include "MeshComponent.h"
#include "MeshRenderCommand.h"
#include "MeshUtils.h"
#include "TransformComponent.h"
#include "WaterComponent.h"

namespace ddr
{
	static dd::Service<dd::JobSystem> s_jobSystem;

	WaterRenderer::WaterRenderer() :
		Renderer( "Lines" )
	{
		RequireTag( ddc::Tag::Visible );
		Require<dd::WaterComponent>();
		Require<dd::TransformComponent>();
		Require<dd::BoundBoxComponent>();
		Require<dd::ColourComponent>();
	}

	void WaterRenderer::RenderInit( ddc::World& world )
	{
	}

	void WaterRenderer::RenderUpdate( ddc::World& world )
	{
		world.ForAllWith<dd::WaterComponent>( [&world]( ddc::Entity entity, dd::WaterComponent& water )
		{
			if( water.Dirty && water.Mesh.IsValid() )
			{
				ddr::Mesh* mesh = water.Mesh.Access();
				mesh->SetPositions( dd::ConstBuffer<glm::vec3>( water.Vertices ) );
				mesh->SetIndices( dd::ConstBuffer<uint>( dd::MeshUtils::GetGridIndices( dd::WaterComponent::VertexCount, water.LOD ) ) );

				water.Dirty = false;
			}
		} );
	}

	void WaterRenderer::Render( const RenderData& render_data )
	{
		auto waters = render_data.Get<dd::WaterComponent>();
		auto transforms = render_data.Get<dd::TransformComponent>();
		auto bound_boxes = render_data.Get<dd::BoundBoxComponent>();

		const ICamera& camera = render_data.Camera();

		UniformStorage& uniforms = render_data.Uniforms();

		for( size_t i = 0; i < render_data.Size(); ++i )
		{
			if( !waters[i].Mesh.IsValid() )
				continue;

			ddr::Mesh* mesh = waters[i].Mesh.Access();
			mesh->Update( *s_jobSystem );

			ddm::AABB transformed = bound_boxes[i].BoundBox.GetTransformed( transforms[i].Transform() );

			glm::vec3 closest = glm::clamp( camera.GetPosition(), transformed.Min, transformed.Max );

			MeshRenderCommand* cmd;
			render_data.Commands().Allocate( cmd );

			cmd->Material = mesh->GetMaterial();
			cmd->Mesh = waters[i].Mesh;
			cmd->Transform = transforms[i].Transform();
			cmd->Colour = glm::vec4( 0, 0, 1, 0.5 );
		}
	}
}
