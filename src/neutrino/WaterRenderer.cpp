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
#include "MeshRenderCommand.h"
#include "MeshUtils.h"
#include "TransformComponent.h"

#include "neutrino/WaterComponent.h"

namespace neut
{
	static dd::Service<dd::JobSystem> s_jobSystem;
	static dd::Service<ddr::ShaderManager> s_shaderManager;
	static dd::Service<ddr::MaterialManager> s_materialManager;

	WaterRenderer::WaterRenderer() :
		IRenderer( "Water" )
	{
		RequireTag( ddc::Tag::Visible );
		Require<neut::WaterComponent>();
	}

	void WaterRenderer::Initialize()
	{
		ddr::MaterialHandle material_h = s_materialManager->Create("water");

		ddr::Material* material = material_h.Access();
		material->Shader = s_shaderManager->Load("water");
		material->State.BackfaceCulling = true;
		material->State.Blending = true;
		material->State.Depth = true;
		material->State.DepthWrite = false;
	}

	void WaterRenderer::Update(ddr::RenderData& render_data)
	{
		ddc::EntityLayer& layer = render_data.EntityLayer();

		layer.ForAllWith<neut::WaterComponent>( []( ddc::Entity entity, neut::WaterComponent& water )
		{
			if( water.Dirty && water.Mesh.IsValid() )
			{
				ddr::Mesh* mesh = water.Mesh.Access();
				mesh->SetPositions( dd::ConstBuffer<glm::vec3>( water.Vertices ) );
				mesh->SetIndices( dd::ConstBuffer<uint>( dd::MeshUtils::GetGridIndices( neut::WaterComponent::VertexCount, water.LOD ) ) );

				water.Dirty = false;
			}
		} );
	}

	void WaterRenderer::Render( const ddr::RenderData& render_data )
	{
		DD_TODO("Removed mesh render command here.");
	}
}
