//
// MeshRenderer.cpp - Renderer class for entities that have mesh components.
// Copyright (C) Sebastian Nordgren 
// July 17th 2018
//

#include "PCH.h"
#include "MeshRenderer.h"

#include "BoundBoxComponent.h"
#include "ColourComponent.h"
#include "Frustum.h"
#include "ICamera.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MeshUtils.h"
#include "MousePicking.h"
#include "OpenGL.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TransformComponent.h"
#include "Uniforms.h"

namespace ddr
{
	MeshRenderer::MeshRenderer( const dd::MousePicking& mousePicking ) :
		ddr::Renderer( "Meshes" ),
		m_mousePicking( mousePicking )
	{
		RequireTag( ddc::Tag::Visible );
		Require<dd::MeshComponent>();
		Require<dd::BoundBoxComponent>();
		Require<dd::TransformComponent>();
		Require<dd::ColourComponent>();
	}

	void MeshRenderer::RenderInit( ddc::World& world )
	{
		m_cube = Mesh::Find( "cube" );
		if( !m_cube.IsValid() )
		{
			m_cube = Mesh::Create( "cube" );

			Mesh* mesh = Mesh::Get( m_cube );

			ShaderHandle shader_h = ShaderProgram::Load( "mesh" );
			ShaderProgram* shader = ShaderProgram::Get( shader_h );

			MaterialHandle material_h = Material::Create( "mesh" );
			Material* material = Material::Get( material_h );
			material->SetShader( shader_h );

			mesh->SetMaterial( material_h );

			shader->Use( true );

			dd::MakeUnitCube( *mesh );

			shader->Use( false );
		}
	}

	void MeshRenderer::Render( const ddr::RenderData& data )
	{
		m_meshCount = 0;
		m_unculledMeshCount = 0;

		if( m_depthTest )
		{
			// depth test
			glEnable( GL_DEPTH_TEST );
			glDepthFunc( GL_GREATER );
			glClipControl( GL_LOWER_LEFT, GL_ZERO_TO_ONE );
		}
		else
		{
			glDisable( GL_DEPTH_TEST );
		}

		// backface culling
		if( m_backfaceCulling )
		{
			glEnable( GL_CULL_FACE );
			glFrontFace( GL_CCW );
			glCullFace( GL_BACK );
		}
		else
		{
			glDisable( GL_CULL_FACE );
		}

		ddr::UniformStorage& uniforms = data.Uniforms();
		uniforms.Set( "DrawNormals", m_drawNormals );

		auto meshes = data.Get<dd::MeshComponent>();
		auto bounds = data.Get<dd::BoundBoxComponent>();
		auto transforms = data.Get<dd::TransformComponent>();
		auto colours = data.Get<dd::ColourComponent>();

		auto entities = data.Entities();
		
		for( size_t i = 0; i < data.Size(); ++i )
		{
			RenderMesh( entities[i], meshes[i], transforms[i], bounds[i], colours[i], data.World(), data.Camera(), uniforms );
		}
	}

	void MeshRenderer::RenderMesh( ddc::Entity entity, const dd::MeshComponent& mesh_cmp, const dd::TransformComponent& transform_cmp, 
		const dd::BoundBoxComponent& bounds_cmp, const dd::ColourComponent& colour_cmp,
		const ddc::World& world, const ddr::ICamera& camera, ddr::UniformStorage& uniforms )
	{
		Mesh* mesh = nullptr;
		mesh = Mesh::Get( mesh_cmp.Mesh );

		DD_ASSERT( mesh != nullptr, "Invalid mesh given!" );

		if( mesh->IsDirty() )
		{
			mesh->UpdateBuffers();
		}

		++m_meshCount;

		dd::AABB world_bounds = bounds_cmp.BoundBox.GetTransformed( transform_cmp.Transform );
		DD_ASSERT( world_bounds.IsValid() );
		
		// check if it intersects with the frustum
		if( m_frustumCull && !camera.GetFrustum().Intersects( world_bounds ) )
		{
			return;
		}

		++m_unculledMeshCount;

		glm::vec4 debugMultiplier( 1, 1, 1, 1 );

		if( world.HasTag( entity, ddc::Tag::Focused ) )
		{
			debugMultiplier.z = 1.5f;
		}

		if( world.HasTag( entity, ddc::Tag::Selected ) )
		{
			debugMultiplier.y = 1.5f;
		}

		if( m_debugHighlightFrustumMeshes )
		{
			debugMultiplier.x = 1.5f;
		}

		Material* material = Material::Get( mesh->GetMaterial() );
		DD_ASSERT( material != nullptr );

		ShaderProgram* shader = ShaderProgram::Get( material->GetShader() );
		DD_ASSERT( shader != nullptr );

		shader->Use( true );

		material->UpdateUniforms( uniforms );

		glm::vec4 colour = colour_cmp.Colour * debugMultiplier;
		uniforms.Set( "ObjectColour", colour );

		uniforms.Bind( *shader );

		mesh->Render( uniforms, *shader, transform_cmp.Transform );

		shader->Use( false );
	}

	void MeshRenderer::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::Value( "Meshes", m_meshCount );
		ImGui::Value( "Unculled Meshes", m_unculledMeshCount );

		ImGui::Checkbox( "Frustum Culling", &m_frustumCull );
		ImGui::Checkbox( "Highlight Frustum Meshes", &m_debugHighlightFrustumMeshes );

		ImGui::Checkbox( "Backface Culling", &m_backfaceCulling );
		ImGui::Checkbox( "Depth Test", &m_depthTest );

		ImGui::Checkbox( "Draw Normals", &m_drawNormals );
	}
}