//
// WaterRenderer.h - Renderer for water.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#include "PCH.h"
#include "WaterRenderer.h"

#include "BoundBoxComponent.h"
#include "ColourComponent.h"
#include "MeshComponent.h"
#include "MeshRenderCommand.h"
#include "MeshUtils.h"
#include "TransformComponent.h"
#include "WaterComponent.h"

namespace ddr
{
	ddr::MaterialHandle WaterRenderer::s_material;

	WaterRenderer::WaterRenderer() :
		Renderer( "Lines" )
	{
		Require<dd::WaterComponent>();
		Require<dd::TransformComponent>();
		Require<dd::BoundBoxComponent>();
		Require<dd::ColourComponent>();
		RequireTag( ddc::Tag::Visible );

		m_renderState.BackfaceCulling = true;
		m_renderState.Blending = true;
		m_renderState.Depth = true;
		m_renderState.DepthWrite = false;
	}

	void WaterRenderer::RenderInit( ddc::World& world )
	{
		s_material = ddr::MaterialManager::Instance()->Create( "water" );

		ddr::Material* material = s_material.Access();
		material->Shader = ddr::ShaderManager::Instance()->Load( "water" );
	}

	void WaterRenderer::RenderUpdate( ddc::World& world )
	{
		world.ForAllWith<dd::WaterComponent>( [&world]( ddc::Entity entity, dd::WaterComponent& water )
		{
			if( water.Dirty && water.Mesh.IsValid() )
			{
				ddr::Mesh* mesh = water.Mesh.Access();
				mesh->SetPositions( dd::ConstBuffer<glm::vec3>( water.Vertices ) );
				mesh->SetIndices( dd::ConstBuffer<uint>( dd::GetGridIndices( dd::WaterComponent::VertexCount, water.LOD ) ) );

				water.Dirty = false;
			}
		} );
	}

	struct MeshEntry
	{
		float Distance2;
		ddr::MeshHandle Mesh;
		glm::mat4 Transform;
		glm::vec4 Colour;
	};

	static std::vector<MeshEntry> s_meshes;

	void WaterRenderer::Render( const RenderData& render_data )
	{
		auto waters = render_data.Get<dd::WaterComponent>();
		auto transforms = render_data.Get<dd::TransformComponent>();
		auto bound_boxes = render_data.Get<dd::BoundBoxComponent>();

		const ICamera& camera = render_data.Camera();

		s_meshes.clear();
		s_meshes.reserve( render_data.Size() );

		for( size_t i = 0; i < render_data.Size(); ++i )
		{
			if( !waters[i].Mesh.IsValid() )
				continue;

			ddm::AABB transformed = bound_boxes[i].BoundBox.GetTransformed( transforms[i].Transform() );

			glm::vec3 closest = glm::clamp( camera.GetPosition(), transformed.Min, transformed.Max );
			MeshEntry entry;
			entry.Mesh = waters[ i ].Mesh;
			entry.Distance2 = glm::distance2( camera.GetPosition(), closest );
			entry.Transform = transforms[i].Transform();

			s_meshes.push_back( entry );
		}

		std::sort( s_meshes.begin(), s_meshes.end(), 
			[]( const auto& a, const auto& b )
		{
			return a.Distance2 > b.Distance2;
		} );

		UniformStorage& uniforms = render_data.Uniforms();

		for( auto entry : s_meshes )
		{
			uniforms.Set( "ObjectColour", entry.Colour );
			uniforms.Set( "Model", entry.Transform );

			MeshRenderCommand* cmd;
			render_data.Commands().Allocate( cmd );

			cmd->Mesh = entry.Mesh;
			cmd->Transform = entry.Transform;
		}
	}
}
