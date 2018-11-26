//
// RayRenderer.cpp - Renderer for RayComponent.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#include "PCH.h"
#include "RayRenderer.h"

#include "ColourComponent.h"
#include "RayComponent.h"
#include "ShaderProgram.h"

namespace ddr
{
	static const glm::vec3 s_lines[] =
	{
		// direction
		glm::vec3( 0, 0, 0 ),
		glm::vec3( 0, 0, 1 ),

		// arrowhead
		glm::vec3( 0, 0, 1 ),
		glm::vec3( 0, 0.05, 0.95 ),
		glm::vec3( 0, 0, 1 ),
		glm::vec3( 0, -0.05, 0.95 ),
		glm::vec3( 0, 0, 1 ),
		glm::vec3( 0.05, 0, 0.95 ),
		glm::vec3( 0, 0, 1 ),
		glm::vec3( -0.05, 0, 0.95 )
	};

	static const dd::ConstBuffer<glm::vec3> s_linesBuffer( s_lines, dd::ArrayLength( s_lines ) );

	RayRenderer::RayRenderer() :
		Renderer( "Ray" )
	{
		RequireTag( ddc::Tag::Visible );
		Require<dd::RayComponent>();
		Optional<dd::ColourComponent>();
	}

	RayRenderer::~RayRenderer()
	{

	}

	void RayRenderer::RenderInit( ddc::World& world )
	{
		m_shader = ShaderManager::Instance()->Load( "line" );
		DD_ASSERT( m_shader.IsValid() );

		ShaderProgram* shader = m_shader.Access();
		DD_ASSERT( shader != nullptr );

		shader->Use( true );

		m_vao.Create();
		m_vao.Bind();

		m_vbo.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vbo.Bind();
		m_vbo.SetData( s_linesBuffer );
		m_vbo.CommitData();

		shader->BindPositions();

		m_vbo.Unbind();
		m_vao.Unbind();

		shader->Use( false );
	}

	void RayRenderer::Render( const ddr::RenderData& data )
	{
		ShaderProgram* shader = m_shader.Access();
		DD_ASSERT( shader != nullptr );
		
		shader->Use( true );

		m_vao.Bind();
		m_vbo.Bind();

		glm::mat4 view_projection = data.Camera().GetProjectionMatrix() * data.Camera().GetViewMatrix();

		auto rays = data.Get<dd::RayComponent>();
		auto colours = data.Get<dd::ColourComponent>();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			const dd::RayComponent& ray = rays[ i ];

			glm::vec4 clr( 1 );

			const dd::ColourComponent* colour = colours.Get( i );
			if( colour != nullptr )
			{
				clr = colour->Colour;
			}

			shader->SetUniform( "Colour", clr );

			float scale = ray.Ray.HasLength() ? ray.Ray.Length : 100;
			glm::mat4 model = ddm::TransformFromRay( ray.Ray ) * glm::scale( glm::vec3( scale ) );

			shader->SetUniform( "ModelViewProjection", view_projection * model );

			glDrawArrays( GL_LINES, 0, m_vbo.GetDataSize() );
		}

		m_vbo.Unbind();
		m_vao.Unbind();

		shader->Use( false );
	}
}