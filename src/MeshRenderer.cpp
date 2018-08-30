//
// MeshRenderer.cpp - Renderer class for entities that have mesh components.
// Copyright (C) Sebastian Nordgren 
// July 17th 2018
//

#include "PrecompiledHeader.h"
#include "MeshRenderer.h"

#include "EntityManager.h"
#include "Frustum.h"
#include "ICamera.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MousePicking.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TransformComponent.h"
#include "Uniforms.h"

#include "imgui/imgui.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace ddr
{
	MeshRenderer::MeshRenderer( const dd::MousePicking& mousePicking ) :
		m_mousePicking( mousePicking )
	{

	}

	void MeshRenderer::RenderInit()
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

		DD_TODO( "Uncomment" );
		/*entity_manager.ForAllWithReadable<dd::MeshComponent, dd::TransformComponent>( [this, &camera, &uniforms]( auto entity, auto mesh, auto transform )
		{
			RenderMesh( entity, *mesh.Read(), *transform.Read(), camera, uniforms );
		} );*/
	}

	void MeshRenderer::RenderMesh( ddc::Entity entity, const dd::MeshComponent& mesh_cmp, const dd::TransformComponent& transform_cmp,
		const ddr::ICamera& camera, ddr::UniformStorage& uniforms )
	{
		if( mesh_cmp.Hidden )
		{
			return;
		}

		Mesh* mesh = nullptr;

		if( m_debugDrawBounds )
		{
			mesh = Mesh::Get( m_unitCube );
		}
		else
		{
			mesh = Mesh::Get( mesh_cmp.Mesh );
		}

		if( mesh == nullptr )
		{
			return;
		}

		++m_meshCount;

		// check if it intersects with the frustum
		if( m_frustumCull && !camera.GetFrustum().Intersects( mesh_cmp.Bounds ) )
		{
			return;
		}

		++m_unculledMeshCount;

		glm::mat4 transform = transform_cmp.World;

		if( m_debugDrawBounds )
		{
			glm::vec3 scale = mesh_cmp.Bounds.Max - mesh_cmp.Bounds.Min;
			transform = glm::translate( mesh_cmp.Bounds.Center() ) * glm::scale( scale / 2.0f );
		}

		glm::vec4 debugMultiplier( 1, 1, 1, 1 );

		if( entity == m_mousePicking.GetFocusedMesh() )
		{
			debugMultiplier.z = 1.5f;
		}

		if( entity == m_mousePicking.GetSelectedMesh() )
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

		mesh->Render( uniforms, *shader, transform );

		shader->Use( false );
	}

	void MeshRenderer::DrawDebugInternal()
	{
		ImGui::Value( "Meshes", m_meshCount );
		ImGui::Value( "Unculled Meshes", m_unculledMeshCount );

		ImGui::Checkbox( "Draw Bounds", &m_debugDrawBounds );

		ImGui::Checkbox( "Frustum Culling", &m_frustumCull );
		ImGui::Checkbox( "Highlight Frustum Meshes", &m_debugHighlightFrustumMeshes );
	}
}