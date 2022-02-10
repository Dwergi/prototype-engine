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
#include "MeshComponent.h"
#include "MeshUtils.h"
#include "TransformComponent.h"

#include "ddr/Material.h"
#include "ddr/MeshRenderCommand.h"
#include "ddr/OpenGL.h"
#include "ddr/ShaderPart.h"
#include "ddr/Shader.h"
#include "ddr/Uniforms.h"

static dd::Service<dd::JobSystem> s_jobsystem;
static dd::Service<ddr::MeshManager> s_meshManager;
static dd::ProfilerValueRef s_meshesRendered("Meshes Rendered");
static dd::Service<ddr::MeshRenderCommandBuffer> s_commands;

namespace ddr
{
	MeshRenderer::MeshRenderer() :
		ddr::IRenderer("Meshes")
	{
		RequireTag(ddc::Tag::Visible);
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

		dd::Services::Register(new MeshRenderCommandBuffer());
	}

	void MeshRenderer::Update(ddr::RenderData& data)
	{
		size_t count = s_meshManager->LiveCount();

		for (size_t i = 0; i < count; ++i)
		{
			Mesh* mesh = s_meshManager->AccessNth(i);
			mesh->Update(*s_jobsystem);
		}
	}

	void MeshRenderer::Render(const ddr::RenderData& data)
	{
		m_meshCount = 0;
		m_unculledMeshCount = 0;
		m_commandCount = 0;

		ddr::UniformStorage& uniforms = data.Uniforms();
		uniforms.Set("DrawNormals", m_drawNormals);

		auto meshes = data.Get<dd::MeshComponent>();
		auto bound_boxes = data.Get<dd::BoundBoxComponent>();
		auto bound_spheres = data.Get<dd::BoundSphereComponent>();
		auto transforms = data.Get<dd::TransformComponent>();
		auto colours = data.Get<dd::ColourComponent>();

		const auto& entities = data.Entities();

		for (size_t i = 0; i < data.Size(); ++i)
		{
			RenderMesh(entities[i], meshes[i], transforms[i], bound_boxes.Get(i), bound_spheres.Get(i), colours.Get(i), data);
		}

		ProcessCommands(data.Uniforms());
	}

	void MeshRenderer::RenderMesh(ddc::Entity entity, const dd::MeshComponent& mesh_cmp, const dd::TransformComponent& transform_cmp,
		const dd::BoundBoxComponent* bbox_cmp, const dd::BoundSphereComponent* bsphere_cmp, const dd::ColourComponent* colour_cmp,
		const ddr::RenderData& render_data)
	{
		if (!mesh_cmp.Mesh.IsValid())
			return;

		const ddr::ICamera& camera = render_data.Camera();
		const ddc::EntityLayer& entities = render_data.Layer();
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

		glm::vec4 debug_tint(0,0,0,1);

		if (entities.HasTag(entity, ddc::Tag::Focused))
		{
			debug_tint.z = 0.5f;
		}

		if (entities.HasTag(entity, ddc::Tag::Selected))
		{
			debug_tint.y = 0.5f;
		}

		if (m_debugHighlightFrustumMeshes)
		{
			debug_tint.x = 0.5f;
		}

		glm::vec4 colour(1);

		if (colour_cmp != nullptr)
		{
			colour = colour_cmp->Colour;
		}

		MeshRenderCommand& cmd = s_meshCommands->Allocate();
		cmd.Material = mesh_cmp.Material;
		cmd.Mesh = mesh_cmp.Mesh;
		cmd.Colour = colour + debug_tint;
		cmd.Transform = transform_cmp.Transform();
		cmd.InitializeKey(camera);
	}

	MeshRenderer::InstanceVBOs& MeshRenderer::FindCachedInstanceVBOs(MeshHandle mesh_h, MaterialHandle material_h)
	{
		uint64 key = mesh_h.GetID();
		key = key << 32;
		key = key | material_h.GetID();

		auto it_pair = m_instanceCache.insert({ key, InstanceVBOs() });
		return it_pair.first->second;
	}

	void MeshRenderer::DrawMeshInstances(MeshHandle mesh_h, MaterialHandle material_h, const std::vector<glm::mat4>& transforms, const std::vector<glm::vec4>& colours)
	{
		DD_ASSERT(transforms.size() == colours.size());

		Mesh* mesh = mesh_h.Access();

		InstanceVBOs& instance_vbos = FindCachedInstanceVBOs(mesh_h, material_h);
		if (instance_vbos.Transforms.IsValid())
		{
			instance_vbos.Transforms.Destroy();
		}
		instance_vbos.Transforms.Create(transforms);
		mesh->VAO().BindVBO(instance_vbos.Transforms, 0, sizeof(transforms[0]));

		if (instance_vbos.Colours.IsValid())
		{
			instance_vbos.Colours.Destroy();
		}
		instance_vbos.Colours.Create(colours);
		mesh->VAO().BindVBO(instance_vbos.Colours, 0, sizeof(colours[0]));

		Shader* shader = material_h->Shader.Access();

		shader->BindAttributeMat4(mesh->VAO(), instance_vbos.Transforms, "TransformInstanced", Normalized::No, Instanced::Yes);
		shader->BindAttributeVec4(mesh->VAO(), instance_vbos.Colours, "ColourInstanced", Normalized::No, Instanced::Yes);

		if (mesh.GetIndices().IsValid())
		{
			OpenGL::DrawElementsInstanced(OpenGL::Primitive::Triangles, mesh.GetIndices().Size(), transforms.size());
		}
		else
		{
			OpenGL::DrawArraysInstanced(OpenGL::Primitive::Triangles, mesh.GetPositions().Size(), transforms.size());
		}
	}

	void MeshRenderer::ProcessCommands(ddr::UniformStorage& uniforms)
	{
		DD_PROFILE_SCOPED(ProcessCommands);

		s_meshCommands->Sort();

		static std::vector<glm::mat4> s_transforms;
		s_transforms.reserve(s_meshCommands->Size());

		static std::vector<glm::vec4> s_colours;
		s_colours.reserve(s_meshCommands->Size());

		MaterialHandle current_mat_h;
		MeshHandle current_mesh_h;

		ddr::MeshRenderCommandBuffer& command_buffer = *s_meshCommands;
		for (const MeshRenderCommand& cmd : command_buffer)
		{
			// switch material and draw if changed
			if (cmd.Mesh != current_mesh_h)
			{
				if (current_mesh_h.IsValid())
				{
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

					DrawMeshInstances(current_mesh_h, current_mat_h, s_transforms, s_colours);
					s_transforms.clear();
					s_colours.clear();
				}

				current_mesh_h = cmd.Mesh;
			}

			s_transforms.push_back(cmd.Transform);
			s_colours.push_back(cmd.Colour);
		}

		if (current_mesh_h.IsValid())
		{
			DrawMeshInstances(current_mesh_h, current_mat_h, s_transforms, s_colours);
		}

		if (current_mat_h.IsValid())
		{
			current_mat_h->Unbind(uniforms);
		}

		s_transforms.clear();
		s_colours.clear();
		command_buffer.Clear();
	}

	void MeshRenderer::DrawDebugInternal()
	{
		ImGui::Value("Mesh Commands", m_commandCount);
		ImGui::Value("Meshes", m_meshCount);
		ImGui::Value("Unculled Meshes", m_unculledMeshCount);

		ImGui::Checkbox("Frustum Culling", &m_frustumCull);
		ImGui::Checkbox("Highlight Frustum Meshes", &m_debugHighlightFrustumMeshes);

		MaterialHandle material_h("mesh");
		Material* material = material_h.Access();

		bool culling = material->State.BackfaceCulling;
		if (ImGui::Checkbox("Backface Culling", &culling))
		{
			material->State.BackfaceCulling = culling;
		}

		bool depth = material->State.Depth;
		if (ImGui::Checkbox("Depth Test", &depth))
		{
			material->State.Depth = depth;
		}

		ImGui::Checkbox("Draw Normals", &m_drawNormals);
	}
}
