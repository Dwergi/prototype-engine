//
// MeshRenderer.h - Renderer class for entities that have mesh components.
// Copyright (C) Sebastian Nordgren 
// July 17th 2018
//

#pragma once

#include "Entity.h"
#include "IDebugPanel.h"
#include "Renderer.h"
#include "MeshHandle.h"

namespace dd
{
	struct BoundsComponent;
	class MeshComponent; 
	class MousePicking;
	struct TransformComponent;
}

namespace ddr
{
	class MeshRenderer : public dd::IDebugPanel, public ddr::Renderer
	{
	public:

		MeshRenderer( const dd::MousePicking& m_mousePicking );

		virtual void RenderInit() override;
		virtual void Render( const ddr::RenderData& render_data ) override;

	private:

		int m_meshCount { 0 };
		int m_unculledMeshCount { 0 };
		bool m_debugDrawBounds { false };
		bool m_frustumCull { true };
		bool m_debugHighlightFrustumMeshes { false };

		MeshHandle m_unitCube;
		
		const dd::MousePicking& m_mousePicking;

		void RenderMesh( ddc::Entity entity, const dd::MeshComponent& mesh_cmp, const dd::TransformComponent& transform_cmp,
			const dd::BoundsComponent& bounds_cmp, const ddr::ICamera& camera, ddr::UniformStorage& uniforms );

		virtual void DrawDebugInternal( const ddc::World& world ) override;
		virtual const char* GetDebugTitle() const override { return "Meshes"; }
	};
}