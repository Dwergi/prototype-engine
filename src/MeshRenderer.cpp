//
// MeshRenderer.cpp - Renderer class for entities that have mesh components.
// Copyright (C) Sebastian Nordgren 
// July 17th 2018
//

#include "PrecompiledHeader.h"
#include "MeshRenderer.h"

#include "BoundsComponent.h"
#include "Frustum.h"
#include "ICamera.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MousePicking.h"
#include "OpenGL.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TransformComponent.h"
#include "Uniforms.h"

#include "imgui/imgui.h"



#include "glm/gtc/type_ptr.hpp"

namespace ddr
{
	MeshRenderer::MeshRenderer( const dd::MousePicking& mousePicking ) :
		ddr::Renderer( "Meshes" ),
		m_mousePicking( mousePicking )
	{
		RequireTag( ddc::Tag::Visible );
		Require<dd::MeshComponent>();
		Require<dd::BoundsComponent>();
		Require<dd::TransformComponent>();
	}

	void MeshRenderer::RenderInit( ddc::World& world )
	{
		m_unitCube = Mesh::Find( "unitcube" );
		if( !m_unitCube.IsValid() )
		{
			m_unitCube = Mesh::Create( "unitcube" );

			Mesh* mesh = Mesh::Get( m_unitCube );

			ShaderHandle shader_h = ShaderProgram::Load( "standard" );
			ShaderProgram* shader = ShaderProgram::Get( shader_h );

			MaterialHandle material_h = Material::Create( "standard" );
			Material* material = Material::Get( material_h );
			material->SetShader( shader_h );

			mesh->SetMaterial( material_h );

			shader->Use( true );

			mesh->MakeUnitCube();

			shader->Use( false );
		}
	}

	void MeshRenderer::Render( const ddr::RenderData& data )
	{
		m_meshCount = 0;
		m_unculledMeshCount = 0;

		ddr::RenderBuffer<dd::MeshComponent> meshes = data.Get<dd::MeshComponent>();
		ddr::RenderBuffer<dd::BoundsComponent> bounds = data.Get<dd::BoundsComponent>();
		ddr::RenderBuffer<dd::TransformComponent> transforms = data.Get<dd::TransformComponent>();

		dd::Span<ddc::Entity> entities = data.Entities();
		
		for( size_t i = 0; i < entities.Size(); ++i )
		{
			RenderMesh( entities[i], meshes[i], transforms[i], bounds[i], data.World(), data.Camera(), data.Uniforms() );
		}
	}

	void MeshRenderer::RenderMesh( ddc::Entity entity, const dd::MeshComponent& mesh_cmp, 
		const dd::TransformComponent& transform_cmp, const dd::BoundsComponent& bounds_cmp, 
		const ddc::World& world, const ddr::ICamera& camera, ddr::UniformStorage& uniforms )
	{
		Mesh* mesh = nullptr;
		mesh = Mesh::Get( mesh_cmp.Mesh );

		if( mesh == nullptr )
		{
			return;
		}

		if( mesh->IsDirty() )
		{
			mesh->UpdateBuffers();
		}

		++m_meshCount;

		// check if it intersects with the frustum
		if( m_frustumCull && !camera.GetFrustum().Intersects( bounds_cmp.WorldBox ) )
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

		glm::vec4 colour = mesh_cmp.Colour * debugMultiplier;
		uniforms.Set( "ObjectColour", colour );

		uniforms.Bind( *shader );

		mesh->Render( uniforms, *shader, transform_cmp.World );

		shader->Use( false );
	}

	void MeshRenderer::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::Value( "Meshes", m_meshCount );
		ImGui::Value( "Unculled Meshes", m_unculledMeshCount );

		ImGui::Checkbox( "Frustum Culling", &m_frustumCull );
		ImGui::Checkbox( "Highlight Frustum Meshes", &m_debugHighlightFrustumMeshes );
	}
}