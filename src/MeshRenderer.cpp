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
#include "MeshRenderCommand.h"
#include "MeshUtils.h"
#include "OpenGL.h"
#include "Services.h"
#include "ShaderPart.h"
#include "Shader.h"
#include "TransformComponent.h"
#include "Uniforms.h"

static dd::Service<dd::JobSystem> s_jobsystem;
static dd::Service<ddr::MeshManager> s_meshManager;

namespace ddr
{
	MeshRenderer::MeshRenderer() :
		ddr::Renderer( "Meshes" )
	{
		RequireTag( ddc::Tag::Visible );
		Require<dd::MeshComponent>();
		Require<dd::TransformComponent>();
		Optional<dd::ColourComponent>();
		Optional<dd::BoundBoxComponent>();
		Optional<dd::BoundSphereComponent>();
	}

	void MeshRenderer::RenderInit( ddc::EntitySpace& entities )
	{
		dd::MeshUtils::CreateDefaultMaterial();
		dd::MeshUtils::CreateUnitCube();
		dd::MeshUtils::CreateUnitSphere();
		dd::MeshUtils::CreateQuad();
	}

	void MeshRenderer::RenderUpdate( ddc::EntitySpace& entities )
	{
		size_t count = s_meshManager->LiveCount();
		
		for( size_t i = 0; i < count; ++i )
		{
			Mesh* mesh = s_meshManager->AccessNth( i );
			mesh->Update( *s_jobsystem );
		}
	}

	void MeshRenderer::Render( const ddr::RenderData& data )
	{
		m_meshCount = 0;
		m_unculledMeshCount = 0;
		
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
			RenderMesh( entities[i], meshes[i], transforms[i], bound_boxes.Get( i ), bound_spheres.Get( i ), colours.Get( i ), data );
		}
	}

	void MeshRenderer::RenderMesh( ddc::Entity entity, const dd::MeshComponent& mesh_cmp, const dd::TransformComponent& transform_cmp, 
		const dd::BoundBoxComponent* bbox_cmp, const dd::BoundSphereComponent* bsphere_cmp, const dd::ColourComponent* colour_cmp, 
		const ddr::RenderData& render_data )
	{
		if( !mesh_cmp.Mesh.IsValid() )
			return;

		const ddr::ICamera& camera = render_data.Camera();
		const ddc::EntitySpace& entities = render_data.EntitySpace();
		ddr::UniformStorage& uniforms = render_data.Uniforms();

		++m_meshCount;

		if (bbox_cmp != nullptr || bsphere_cmp != nullptr)
		{
			ddm::AABB world_aabb;
			ddm::Sphere world_sphere;
			if (!dd::GetWorldBoundBoxAndSphere(bbox_cmp, bsphere_cmp, transform_cmp, world_aabb, world_sphere))
			{
				return;
			}

			// check if it intersects with the frustum
			if (m_frustumCull &&
				!camera.GetFrustum().Intersects(world_sphere) &&
				!camera.GetFrustum().Intersects(world_aabb))
			{
				return;
			}
		}

		++m_unculledMeshCount;

		glm::vec4 debug_multiplier( 1, 1, 1, 1 );

		if( entities.HasTag( entity, ddc::Tag::Focused ) )
		{
			debug_multiplier.z = 1.5f;
		}

		if( entities.HasTag( entity, ddc::Tag::Selected ) )
		{
			debug_multiplier.y = 1.5f;
		}

		if( m_debugHighlightFrustumMeshes )
		{
			debug_multiplier.x = 1.5f;
		}

		glm::vec4 colour( 1 );
		
		if( colour_cmp != nullptr )
		{
			colour = colour_cmp->Colour;
		}

		const Mesh* mesh = mesh_cmp.Mesh.Get();
		
		MeshRenderCommand* cmd;
		render_data.Commands().Allocate( cmd );

		cmd->Material = mesh->GetMaterial();
		cmd->Mesh = mesh_cmp.Mesh;
		cmd->Colour = colour * debug_multiplier;
		cmd->Transform = transform_cmp.Transform();
	}

	void MeshRenderer::DrawDebugInternal()
	{
		ImGui::Value( "Meshes", m_meshCount );
		ImGui::Value( "Unculled Meshes", m_unculledMeshCount );

		ImGui::Checkbox( "Frustum Culling", &m_frustumCull );
		ImGui::Checkbox( "Highlight Frustum Meshes", &m_debugHighlightFrustumMeshes );

		MaterialHandle material_h( "mesh" );
		Material* material = material_h.Access();

		bool culling = material->State.BackfaceCulling;
		if( ImGui::Checkbox( "Backface Culling", &culling ) )
		{
			material->State.BackfaceCulling = culling;
		}

		bool depth = material->State.Depth;
		if( ImGui::Checkbox( "Depth Test", &depth ) )
		{
			material->State.Depth = depth;
		}

		ImGui::Checkbox( "Draw Normals", &m_drawNormals );
	}
}
