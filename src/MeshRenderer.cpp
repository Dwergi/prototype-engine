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

#include "fmt/format.h"

static dd::Service<dd::JobSystem> s_jobsystem;
static dd::Service<ddr::MeshManager> s_meshManager;
static dd::Service<ddr::MeshRenderCommandBuffer> s_meshCommands;
static dd::ProfilerValueRef s_meshesRendered("Meshes Rendered");

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

		dd::Services::Register(new MeshRenderCommandBuffer);
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

		glm::vec4 debug_tint(0,0,0,0);

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
		cmd.Colour = glm::max(colour + debug_tint, 1.0f);
		cmd.Transform = transform_cmp.Transform();
		cmd.InitializeKey(camera);
	}

	MeshRenderer::InstanceData& MeshRenderer::FindOrAddInstanceData(MeshHandle mesh_h, MaterialHandle material_h, bool& out_added)
	{
		uint64 key = mesh_h.GetID();
		key = key << 32;
		key = key | material_h.GetID();

		auto it_pair = m_instanceCache.insert({ key, InstanceData() });
		out_added = it_pair.second;
		
		return it_pair.first->second; 
	}
	
	void MeshRenderer::DrawMeshInstances(MeshHandle mesh_h, MaterialHandle material_h, const std::vector<glm::mat4>& transforms, const std::vector<glm::vec4>& colours)
	{
		DD_ASSERT(transforms.size() == colours.size());
		
		if (transforms.size() == 0)
		{
			return;
		}

		Mesh& mesh = *mesh_h.Access();

		// TODO: Should be able to specify this VAO entirely separately from VBOs and just bind buffers before drawing. 

		bool added;
		InstanceData& instance_data = FindOrAddInstanceData(mesh_h, material_h, added);
		
		if (added)
		{
			instance_data.VAO.Create(fmt::format("{} ({})", mesh_h->Name(), material_h->Name()));

			mesh.Bind(instance_data.VAO, *material_h->Shader);

			instance_data.ColourVBO.Create("colour");
			instance_data.VAO.BindVBO(instance_data.ColourVBO, 0, sizeof(colours[0]));
			material_h->Shader->CreateAttributeVec4(instance_data.VAO, instance_data.ColourVBO, "ColourInstanced", Normalized::No, Instanced::Yes);

			instance_data.TransformVBO.Create("transform");
			instance_data.VAO.BindVBO(instance_data.TransformVBO, 0, sizeof(transforms[0]));
			material_h->Shader->CreateAttributeMat4(instance_data.VAO, instance_data.TransformVBO, "TransformInstanced", Normalized::No, Instanced::Yes);
		}

		instance_data.Transforms = transforms;
		instance_data.TransformVBO.SetData(instance_data.Transforms);

		instance_data.Colours = colours;
		instance_data.ColourVBO.SetData(instance_data.Colours);
		
		instance_data.VAO.Bind();

		if (mesh.GetIndices().IsValid())
		{
			OpenGL::DrawElementsInstanced(ddr::Primitive::Triangles, mesh.GetIndices().Size(), transforms.size());
		}
		else
		{
			OpenGL::DrawArraysInstanced(ddr::Primitive::Triangles, mesh.GetPositions().Size(), transforms.size());
		}

		instance_data.VAO.Unbind();
	}

	void MeshRenderer::ProcessCommands(ddr::UniformStorage& uniforms)
	{
		DD_PROFILE_SCOPED(ProcessCommands);

		ddr::MeshRenderCommandBuffer& command_buffer = *s_meshCommands;

		command_buffer.Sort();

		static std::vector<glm::mat4> s_transforms;
		s_transforms.reserve(command_buffer.Size());

		static std::vector<glm::vec4> s_colours;
		s_colours.reserve(command_buffer.Size());

		MaterialHandle current_material_h;
		MeshHandle current_mesh_h;

		for (const MeshRenderCommand& cmd : command_buffer)
		{
			// switch material and draw if changed
			if (cmd.Mesh != current_mesh_h || cmd.Material != current_material_h)
			{
				if (current_mesh_h.IsValid() && current_material_h.IsValid())
				{
					DrawMeshInstances(current_mesh_h, current_material_h, s_transforms, s_colours);
					s_transforms.clear();
					s_colours.clear();
				}

				if (cmd.Material != current_material_h)
				{
					if (current_material_h.IsValid())
					{
						current_material_h->Unbind(uniforms);
					}

					current_material_h = cmd.Material;
					current_material_h->Bind(uniforms);
				}

				current_mesh_h = cmd.Mesh;
			}

			s_transforms.push_back(cmd.Transform);
			s_colours.push_back(cmd.Colour);
		}

		if (current_mesh_h.IsValid())
		{
			DrawMeshInstances(current_mesh_h, current_material_h, s_transforms, s_colours);
		}

		if (current_material_h.IsValid())
		{
			current_material_h->Unbind(uniforms);
		}

		s_transforms.clear();
		s_colours.clear();
		command_buffer.Clear();
	}

	void MeshRenderer::DrawDebugInternal()
	{
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
