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
#include "ColourComponent.h"
#include "Frustum.h"
#include "ICamera.h"
#include "LinesComponent.h"
#include "TransformComponent.h"

namespace ddr
{
	LinesRenderer::LinesRenderer() :
		Renderer( "Lines" )
	{
		Require<dd::LinesComponent>();
		Require<dd::TransformComponent>();

		Optional<dd::ColourComponent>();
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

		Shader* shader = m_shader.Access();
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
		auto colours = render_data.Get<dd::ColourComponent>();

		const ddr::ICamera& camera = render_data.Camera();

		Shader* shader = m_shader.Access();
		ScopedShader scoped_shader = shader->UseScoped();
		ScopedRenderState scoped_state = m_renderState.UseScoped();

		glm::mat4 view_projection = camera.GetProjectionMatrix() * camera.GetViewMatrix();

		for( size_t i = 0; i < render_data.Size(); ++i )
		{
			/*dd::AABB aabb;
			dd::Sphere sphere;
			if( !dd::GetWorldBoundBoxAndSphere( bound_boxes.Get( i ), bound_spheres.Get( i ), transforms[i], aabb, sphere ) )
			{
				continue;
			}

			if( !camera.GetFrustum().Intersects( sphere ) && !camera.GetFrustum().Intersects( aabb ) )
			{
				continue;
			}*/

			if( lines[i].Points.size() == 0 )
			{
				continue;
			}
			
			glm::vec4 colour( 1 );
			const dd::ColourComponent* colour_cmp = colours.Get( i );
			if( colour_cmp != nullptr )
			{
				colour = colour_cmp->Colour;
			}

			shader->SetUniform( "Colour", colour );
			shader->SetUniform( "ModelViewProjection", view_projection * transforms[i].Transform() );

			m_vao.Bind();

			m_vboPosition.Bind();
			m_vboPosition.SetData( dd::ConstBuffer<glm::vec3>( lines[i].Points ) );
			m_vboPosition.CommitData();

			glDrawArrays( GL_LINES, 0, m_vboPosition.GetDataSize() );

			m_vboPosition.Unbind();

			m_vao.Unbind();
		}
	}
}
