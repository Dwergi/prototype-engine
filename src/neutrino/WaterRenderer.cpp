//
// WaterRenderer.h - Renderer for water.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#include "PCH.h"
#include "neutrino/WaterRenderer.h"

#include "BoundBoxComponent.h"
#include "ColourComponent.h"
#include "JobSystem.h"
#include "MeshComponent.h"
#include "MeshRenderCommand.h"
#include "MeshUtils.h"
#include "TransformComponent.h"

#include "neutrino/WaterComponent.h"

namespace neut
{
	static dd::Service<dd::JobSystem> s_jobSystem;
	static dd::Service<ddr::ShaderManager> s_shaderManager;
	static dd::Service<ddr::MaterialManager> s_materialManager;

	WaterRenderer::WaterRenderer() :
		Renderer( "Lines" )
	{
		RequireTag( ddc::Tag::Visible );
		Require<neut::WaterComponent>();
		Require<dd::TransformComponent>();
		Require<dd::BoundBoxComponent>();
		Require<dd::ColourComponent>();
	}

	void WaterRenderer::RenderInit( ddc::EntitySpace& entities )
	{
		DD_TODO("This should be in renderer, irrelevant to system.");
		ddr::MaterialHandle material_h = s_materialManager->Create("water");

		ddr::Material* material = material_h.Access();
		material->Shader = s_shaderManager->Load("water");
		material->State.BackfaceCulling = true;
		material->State.Blending = true;
		material->State.Depth = true;
		material->State.DepthWrite = false;
	}

	void WaterRenderer::RenderUpdate( ddc::EntitySpace& entities )
	{
		entities.ForAllWith<neut::WaterComponent>( [&entities]( ddc::Entity entity, neut::WaterComponent& water )
		{
			if( water.Dirty && water.Mesh.IsValid() )
			{
				ddr::Mesh* mesh = water.Mesh.Access();
				mesh->SetPositions( dd::ConstBuffer<glm::vec3>( water.Vertices ) );
				mesh->SetIndices( dd::ConstBuffer<uint>( dd::MeshUtils::GetGridIndices( neut::WaterComponent::VertexCount, water.LOD ) ) );

				water.Dirty = false;
			}
		} );
	}

	void WaterRenderer::Render( const ddr::RenderData& render_data )
	{
		auto waters = render_data.Get<neut::WaterComponent>();
		auto transforms = render_data.Get<dd::TransformComponent>();
		auto bound_boxes = render_data.Get<dd::BoundBoxComponent>();

		const ddr::ICamera& camera = render_data.Camera();

		ddr::UniformStorage& uniforms = render_data.Uniforms();

		for( size_t i = 0; i < render_data.Size(); ++i )
		{
			if( !waters[i].Mesh.IsValid() )
				continue;

			ddr::Mesh* mesh = waters[i].Mesh.Access();
			mesh->Update( *s_jobSystem );

			ddm::AABB transformed = bound_boxes[i].BoundBox.GetTransformed( transforms[i].Transform() );

			glm::vec3 closest = glm::clamp( camera.GetPosition(), transformed.Min, transformed.Max );

			ddr::MeshRenderCommand* cmd;
			render_data.Commands().Allocate( cmd );

			cmd->Material = mesh->GetMaterial();
			cmd->Mesh = waters[i].Mesh;
			cmd->Transform = transforms[i].Transform();
			cmd->Colour = glm::vec4( 0, 0, 1, 0.5 );
		}
	}
}
