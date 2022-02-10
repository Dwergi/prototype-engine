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
#include "MeshUtils.h"
#include "TransformComponent.h"

#include "ddr/MeshRenderCommand.h"
#include "ddr/RenderCommandBuffer.h"

#include "neutrino/WaterComponent.h"

namespace neut
{
	static dd::Service<ddr::MeshRenderCommandBuffer> s_meshCommands;
	static dd::Service<dd::JobSystem> s_jobSystem;
	static dd::Service<ddr::ShaderManager> s_shaderManager;
	static dd::Service<ddr::MaterialManager> s_materialManager;

	WaterRenderer::WaterRenderer() :
		IRenderer("Water")
	{
		RequireTag(ddc::Tag::Visible);
		Require<neut::WaterComponent>();
		Require<dd::TransformComponent>();
		Require<dd::BoundBoxComponent>();
	}

	void WaterRenderer::Initialize()
	{
		m_material = s_materialManager->Create("water");

		ddr::Material* material = m_material.Access();
		material->Shader = s_shaderManager->Load("water");
		material->State.BackfaceCulling = true;
		material->State.Blending = true;
		material->State.Depth = true;
		material->State.DepthWrite = false;
	}

	void WaterRenderer::Update(ddr::RenderData& render_data)
	{
		ddc::EntityLayer& layer = render_data.Layer();

		layer.ForAllWith<neut::WaterComponent>([](ddc::Entity entity, neut::WaterComponent& water)
		{
			if (water.Dirty && water.Mesh.IsValid())
			{
				ddr::Mesh* mesh = water.Mesh.Access();
				mesh->SetPositions(dd::ConstBuffer<glm::vec3>(water.Vertices));
				mesh->SetIndices(dd::ConstBuffer<uint>(dd::MeshUtils::GetGridIndices(neut::WaterComponent::VertexCount, water.LOD)));

				water.Dirty = false;
			}
		});
	}

	void WaterRenderer::Render(const ddr::RenderData& render_data)
	{
		auto waters = render_data.Get<neut::WaterComponent>();
		auto transforms = render_data.Get<dd::TransformComponent>();
		auto bound_boxes = render_data.Get<dd::BoundBoxComponent>();

		const ddr::ICamera& camera = render_data.Camera();

		ddr::UniformStorage& uniforms = render_data.Uniforms();

		for (size_t i = 0; i < render_data.Size(); ++i)
		{
			if (!waters[i].Mesh.IsValid())
				continue;

			ddr::Mesh* mesh = waters[i].Mesh.Access();
			mesh->Update(*s_jobSystem);

			ddm::AABB transformed = bound_boxes[i].BoundBox.GetTransformed(transforms[i].Transform());

			glm::vec3 closest = glm::clamp(camera.GetPosition(), transformed.Min, transformed.Max);

			ddr::MeshRenderCommand& cmd = s_meshCommands->Allocate();
			cmd.Mesh = waters[i].Mesh;
			cmd.Material = m_material;
			cmd.Transform = transforms[i].Transform();
			cmd.Colour = glm::vec4(0, 0, 1, 0.5);
			cmd.InitializeKey(camera);
		}
	}
}
