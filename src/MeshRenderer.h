//
// MeshRenderer.h - Renderer class for entities that have mesh components.
// Copyright (C) Sebastian Nordgren 
// July 17th 2018
//

#pragma once

#include "Entity.h"
#include "IDebugPanel.h"
#include "Mesh.h"
#include "Renderer.h"

namespace dd
{
	struct BoundBoxComponent;
	struct ColourComponent;
	struct JobSystem;
	struct MeshComponent; 
	struct TransformComponent;
}

namespace ddr
{
	class MeshRenderer : public dd::IDebugPanel, public ddr::Renderer
	{
	public:
		MeshRenderer( dd::JobSystem& job_system );

		virtual void RenderInit( ddc::World& world ) override;
		virtual void Render( const ddr::RenderData& render_data ) override;

	private:

		int m_meshCount { 0 };
		int m_unculledMeshCount { 0 };
		
		bool m_frustumCull { true };
		bool m_debugHighlightFrustumMeshes { false };

		bool m_depthTest { true };
		bool m_backfaceCulling { true };
		bool m_drawNormals { false };

		MeshHandle m_cube;
		
		dd::JobSystem& m_jobsystem;

		void RenderMesh( ddc::Entity entity, const dd::MeshComponent& mesh_cmp, const dd::TransformComponent& transform_cmp,
			const dd::BoundBoxComponent& bounds_cmp, const dd::ColourComponent& colour_cmp, 
			const ddc::World& world, const ddr::ICamera& camera, ddr::UniformStorage& uniforms );

		virtual void DrawDebugInternal( ddc::World& world ) override;
		virtual const char* GetDebugTitle() const override { return "Meshes"; }
	};
}