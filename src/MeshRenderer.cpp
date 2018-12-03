//
// MeshRenderer.cpp - Renderer class for entities that have mesh components.
// Copyright (C) Sebastian Nordgren 
// July 17th 2018
//

#include "PCH.h"
#include "MeshRenderer.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "BoundsHelpers.h"
#include "ColourComponent.h"
#include "Frustum.h"
#include "ICamera.h"
#include "Material.h"
#include "MeshComponent.h"
#include "MeshUtils.h"
#include "OpenGL.h"
#include "ShaderPart.h"
#include "Shader.h"
#include "TransformComponent.h"
#include "Uniforms.h"

namespace ddr
{
	MeshRenderer::MeshRenderer( dd::JobSystem& jobsystem ) :
		ddr::Renderer( "Meshes" ),
		m_jobsystem( jobsystem )
	{
		RequireTag( ddc::Tag::Visible );
		Require<dd::MeshComponent>();
		Require<dd::TransformComponent>();
		Optional<dd::ColourComponent>();
		Optional<dd::BoundBoxComponent>();
		Optional<dd::BoundSphereComponent>();

		m_state.BackfaceCulling = true;
		m_state.Blending = false;
		m_state.Depth = true;
	}

	void MeshRenderer::RenderInit( ddc::World& world )
	{
		m_cube = MeshManager::Instance()->Find( "cube" );
		if( !m_cube.IsValid() )
		{
			m_cube = MeshManager::Instance()->Create( "cube" );
			Mesh* mesh = m_cube.Access();

			ShaderHandle shader_h = ShaderManager::Instance()->Load( "mesh" );
			Shader* shader = shader_h.Access();

			MaterialHandle material_h = MaterialManager::Instance()->Create( "mesh" );
			Material* material = material_h.Access();
			material->SetShader( shader_h );

			mesh->SetMaterial( material_h );

			shader->Use( true );

			dd::MakeUnitCube( *mesh );

			shader->Use( false );
		}
	}

	void MeshRenderer::RenderUpdate( ddc::World& world )
	{
		size_t count = MeshManager::Instance()->Count();
		
		for( size_t i = 0; i < count; ++i )
		{
			Mesh* mesh = MeshManager::Instance()->AccessAt( i );
			mesh->Update( m_jobsystem );
		}
	}

	void MeshRenderer::Render( const ddr::RenderData& data )
	{
		m_meshCount = 0;
		m_unculledMeshCount = 0;
		
		ScopedRenderState state = m_state.UseScoped();

		ddr::UniformStorage& uniforms = data.Uniforms();
		uniforms.Set( "DrawNormals", m_drawNormals );

		auto meshes = data.Get<dd::MeshComponent>();
		auto bound_boxes = data.Get<dd::BoundBoxComponent>();
		auto bound_spheres = data.Get<dd::BoundSphereComponent>();
		auto transforms = data.Get<dd::TransformComponent>();
		auto colours = data.Get<dd::ColourComponent>();

		auto entities = data.Entities();
		
		for( size_t i = 0; i < data.Size(); ++i )
		{
			RenderMesh( entities[i], meshes[i], transforms[i], bound_boxes.Get( i ), bound_spheres.Get( i ), 
				colours.Get( i ), data.World(), data.Camera(), uniforms );
		}
	}

	void MeshRenderer::RenderMesh( ddc::Entity entity, const dd::MeshComponent& mesh_cmp, const dd::TransformComponent& transform_cmp, 
		const dd::BoundBoxComponent* bbox_cmp, const dd::BoundSphereComponent* bsphere_cmp, const dd::ColourComponent* colour_cmp,
		const ddc::World& world, const ddr::ICamera& camera, ddr::UniformStorage& uniforms )
	{
		Mesh* mesh = mesh_cmp.Mesh.Access();
		if( mesh == nullptr )
		{
			return;
		}

		++m_meshCount;

		ddm::AABB world_aabb;
		ddm::Sphere world_sphere;
		if( !dd::GetWorldBoundBoxAndSphere( bbox_cmp, bsphere_cmp, transform_cmp, world_aabb, world_sphere ) )
		{
			return;
		}

		// check if it intersects with the frustum
		if( m_frustumCull && 
			!camera.GetFrustum().Intersects( world_sphere ) &&
			!camera.GetFrustum().Intersects( world_aabb ) )
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

		const Material* material = mesh->GetMaterial().Get();
		DD_ASSERT( material != nullptr );

		Shader* shader = material->GetShader().Access();
		ScopedShader usage = shader->UseScoped();

		material->UpdateUniforms( uniforms );

		glm::vec4 colour( 1 );
		
		if( colour_cmp != nullptr )
		{
			colour = colour_cmp->Colour;
		}

		uniforms.Set( "ObjectColour", colour * debugMultiplier );

		uniforms.Bind( *shader );

		mesh->Render( uniforms, *shader, transform_cmp.Transform() );
	}

	void MeshRenderer::DrawDebugInternal( ddc::World& world )
	{
		ImGui::Value( "Meshes", m_meshCount );
		ImGui::Value( "Unculled Meshes", m_unculledMeshCount );

		ImGui::Checkbox( "Frustum Culling", &m_frustumCull );
		ImGui::Checkbox( "Highlight Frustum Meshes", &m_debugHighlightFrustumMeshes );

		bool culling = m_state.BackfaceCulling;
		if( ImGui::Checkbox( "Backface Culling", &culling ) )
		{
			m_state.BackfaceCulling = culling;
		}

		bool depth = m_state.Depth;
		if( ImGui::Checkbox( "Depth Test", &depth ) )
		{
			m_state.Depth = depth;
		}

		ImGui::Checkbox( "Draw Normals", &m_drawNormals );
	}
}
