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
static dd::ProfilerValue& s_meshesRendered = dd::Profiler::GetValue("Meshes Rendered");

namespace ddr
{
	MeshRenderer::MeshRenderer() :
		ddr::IRenderer( "Meshes" )
	{
		RequireTag( ddc::Tag::Visible );
		Require<dd::MeshComponent>();
		Require<dd::TransformComponent>();
		Optional<dd::ColourComponent>();
		Optional<dd::BoundBoxComponent>();
		Optional<dd::BoundSphereComponent>();
	}

	void MeshRenderer::Initialize()
	{
		dd::MeshUtils::CreateDefaultMaterial();
		dd::MeshUtils::CreateUnitCube();
		dd::MeshUtils::CreateUnitSphere();
		dd::MeshUtils::CreateQuad();

		m_vboTransforms.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		m_vboColours.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	}

	void MeshRenderer::Update(ddr::RenderData& data)
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
		m_commands.Clear();
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

		ProcessCommands(data.Uniforms());
	}

	void MeshRenderer::RenderMesh( ddc::Entity entity, const dd::MeshComponent& mesh_cmp, const dd::TransformComponent& transform_cmp, 
		const dd::BoundBoxComponent* bbox_cmp, const dd::BoundSphereComponent* bsphere_cmp, const dd::ColourComponent* colour_cmp, 
		const ddr::RenderData& render_data )
	{
		if( !mesh_cmp.Mesh.IsValid() )
			return;

		const ddr::ICamera& camera = render_data.Camera();
		const ddc::EntityLayer& entities = render_data.EntityLayer();
		ddr::UniformStorage& uniforms = render_data.Uniforms();

		++m_meshCount;
		s_meshesRendered.Increment();

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
		
		MeshRenderCommand& cmd = m_commands.Allocate();
		cmd.Material = mesh->GetMaterial();
		cmd.Mesh = mesh_cmp.Mesh;
		cmd.Colour = colour * debug_multiplier;
		cmd.Transform = transform_cmp.Transform();
		cmd.InitializeKey(camera);
	}

	void MeshRenderer::DrawMeshInstances(Mesh* mesh, const std::vector<glm::mat4>& transforms, const std::vector<glm::vec4>& colours)
	{
		DD_ASSERT(mesh != nullptr);
		DD_ASSERT(transforms.size() == colours.size());

		mesh->AccessVAO().Bind();

		const Material* material = mesh->GetMaterial().Get();
		Shader* shader = material->Shader.Access();

		m_vboTransforms.Bind();
		m_vboTransforms.CommitData();
		shader->BindAttributeMat4("TransformInstanced", true);
		m_vboTransforms.Unbind();

		m_vboColours.Bind();
		m_vboColours.CommitData();
		shader->BindAttributeVec4("ColourInstanced", false);
		shader->SetAttributeInstanced("ColourInstanced");
		m_vboColours.Unbind();

		if (mesh->GetIndices().IsValid())
		{
			OpenGL::DrawElementsInstanced(mesh->GetIndices().Size(), (int) transforms.size());
		}
		else
		{
			OpenGL::DrawArraysInstanced(mesh->GetPositions().Size(), (int) transforms.size());
		}

		mesh->AccessVAO().Unbind();
	}

	void MeshRenderer::ProcessCommands(ddr::UniformStorage& uniforms)
	{
		DD_PROFILE_SCOPED(ProcessCommands);

		m_commands.Sort();

		static std::vector<glm::mat4> transforms;
		transforms.reserve(m_commands.Size());
		transforms.clear();

		m_vboTransforms.Bind();
		m_vboTransforms.SetData(transforms.data(), transforms.capacity());
		m_vboTransforms.Unbind();
		
		static std::vector<glm::vec4> colours;
		colours.reserve(m_commands.Size());
		colours.clear();

		m_vboColours.Bind();
		m_vboColours.SetData(colours.data(), colours.capacity());
		m_vboColours.Unbind();

		MaterialHandle current_mat_h;
		MeshHandle current_mesh_h;

		for (int i = 0; i < m_commands.Size(); ++i)
		{
			const MeshRenderCommand& cmd = m_commands.Get(i);
			transforms.push_back(cmd.Transform);
			colours.push_back(cmd.Colour);

			// switch material if changed
			if (cmd.Material != current_mat_h)
			{
				if (current_mat_h.IsValid())
				{
					Material* old_material = current_mat_h.Access();
					old_material->Unbind(uniforms);
				}

				Material* new_material = cmd.Material.Access();
				new_material->Bind(uniforms);

				current_mat_h = cmd.Material;
			}

			// switch mesh and draw if changed
			if (cmd.Mesh != current_mesh_h)
			{
				if (current_mesh_h.IsValid())
				{
					DrawMeshInstances(current_mesh_h.Access(), transforms, colours);
					transforms.clear();
					colours.clear();
				}

				current_mesh_h = cmd.Mesh;
			}
		}

		if (current_mesh_h.IsValid())
		{
			DrawMeshInstances(current_mesh_h.Access(), transforms, colours);
			transforms.clear();
			colours.clear();
		}

		if (current_mat_h.IsValid())
		{
			Material* old_material = current_mat_h.Access();
			old_material->Unbind(uniforms);
		}
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
