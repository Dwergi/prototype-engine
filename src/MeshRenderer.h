//
// MeshRenderer.h - Renderer class for entities that have mesh components.
// Copyright (C) Sebastian Nordgren 
// July 17th 2018
//

#pragma once

#include "CommandBuffer.h"
#include "IDebugPanel.h"
#include "IRenderer.h"
#include "Mesh.h"
#include "MeshRenderCommand.h"

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
	class MeshRenderer : public dd::IDebugPanel, public ddr::IRenderer
	{
	public:
		MeshRenderer();

		virtual void RenderInit( ddc::EntitySpace& entities ) override;
		virtual void RenderUpdate( ddc::EntitySpace& entities ) override;
		virtual void Render( const ddr::RenderData& render_data ) override;

	private:

		int m_meshCount { 0 };
		int m_unculledMeshCount { 0 };
		
		bool m_frustumCull { true };
		bool m_debugHighlightFrustumMeshes { false };
		bool m_drawNormals { false };

		VBO m_vboTransforms;
		VBO m_vboColours;

		MeshHandle m_cube;

		ddr::CommandBuffer<ddr::MeshRenderCommand> m_commands;
		
		void RenderMesh( ddc::Entity entity, const dd::MeshComponent& mesh_cmp, const dd::TransformComponent& transform_cmp,
			const dd::BoundBoxComponent* bound_box, const dd::BoundSphereComponent* bound_sphere, const dd::ColourComponent* colour_cmp,
			const ddr::RenderData& render_data );

		void ProcessCommands(ddr::UniformStorage& uniforms);
		void DrawMeshInstances(Mesh* mesh, const std::vector<glm::mat4>& transforms, const std::vector<glm::vec4>& colours);

		virtual void DrawDebugInternal() override;
		virtual const char* GetDebugTitle() const override { return "Meshes"; }
	};
}