//
// LinesComponent.h - Renderer for lines.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#include "PCH.h"
#include "LinesRenderer.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "BoundsHelpers.h"
#include "LinesComponent.h"
#include "TransformComponent.h"

namespace ddr
{
	LinesRenderer::LinesRenderer() :
		Renderer( "Lines" )
	{
		Require<dd::LinesComponent>();
		Require<dd::TransformComponent>();

		Optional<dd::BoundBoxComponent>();
		Optional<dd::BoundSphereComponent>();

		m_renderState.BackfaceCulling = false;
		m_renderState.Blending = true;
		m_renderState.Depth = true;
	}

	void LinesRenderer::RenderInit( ddc::World& world )
	{
		m_shader = ShaderManager::Instance()->Load( "line" );
		DD_ASSERT( m_shader.IsValid() );

		ShaderProgram* shader = m_shader.Access();
		DD_ASSERT( shader != nullptr );

		ScopedShader scoped_shader = shader->UseScoped();

		m_vao.Create();
		m_vao.Bind();

		m_vboPosition.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboPosition.Bind();
		shader->BindPositions();
		m_vboPosition.Unbind();

		m_vao.Unbind();
	}

	void LinesRenderer::Render( const RenderData& render_data )
	{
		auto lines = render_data.Get<dd::LinesComponent>();
		auto transforms = render_data.Get<dd::TransformComponent>();
		auto bound_boxes = render_data.Get<dd::BoundBoxComponent>();
		auto bound_spheres = render_data.Get<dd::BoundSphereComponent>();

		for( size_t i = 0; i < render_data.Size(); ++i )
		{
			dd::AABB aabb;
			dd::Sphere sphere;
			dd::GetWorldBoundBoxAndSphere( bound_boxes.Get( i ), bound_spheres.Get( i ), transforms[i].Transform(), aabb, sphere );

		}
	}
}
