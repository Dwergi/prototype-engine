//
// MeshRenderer.h - Renderer class for entities that have mesh components.
// Copyright (C) Sebastian Nordgren 
// July 17th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "IRenderer.h"

#include "ddr/Mesh.h"
#include "ddr/MeshRenderCommand.h"
#include "ddr/RenderCommandBuffer.h"

namespace dd
{
	struct BoundBoxComponent;
	struct BoundSphereComponent;
	struct ColourComponent;
	struct JobSystem;
	struct MeshComponent;
	struct TransformComponent;
}

namespace ddr
{
	struct MeshRenderer : public dd::IDebugPanel, public ddr::IRenderer
	{
	public:
		MeshRenderer();

		virtual void Initialize() override;
		virtual void Update(ddr::RenderData& data) override;
		virtual void Render(const ddr::RenderData& render_data) override;

	private:

		struct InstanceData
		{
			VAO VAO;
			std::vector<glm::mat4> Transforms;
			VBO TransformVBO;
			std::vector<glm::vec4> Colours;
			VBO ColourVBO;
			std::string Name;
		};

		std::unordered_map<uint64, InstanceData> m_instanceCache;

		int m_meshCount { 0 };
		int m_unculledMeshCount { 0 };

		bool m_frustumCull { true };
		bool m_debugHighlightFrustumMeshes { false };
		bool m_drawNormals { false };

		MeshHandle m_cube;

		void RenderMesh(ddc::Entity entity, const dd::MeshComponent& mesh_cmp, const dd::TransformComponent& transform_cmp,
			const dd::BoundBoxComponent* bound_box, const dd::BoundSphereComponent* bound_sphere, const dd::ColourComponent* colour_cmp,
			const ddr::RenderData& render_data);

		// 
		// Find or add an instance data to the cache.
		// Returns true if a new instance was added.
		//
		InstanceData& FindOrAddInstanceData(MeshHandle mesh_h, MaterialHandle material_h, bool& out_added);

		void ProcessCommands(ddr::UniformStorage& uniforms);
		void DrawMeshInstances(MeshHandle mesh_h, MaterialHandle material_h, const std::vector<glm::mat4>& transforms, const std::vector<glm::vec4>& colours);

		virtual void DrawDebugInternal() override;
		virtual const char* GetDebugTitle() const override { return "Meshes"; }
	};
}
