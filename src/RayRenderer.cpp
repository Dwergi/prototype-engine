#include "PrecompiledHeader.h"
#include "RayRenderer.h"

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

	static const dd::ConstBuffer<glm::vec3> s_linesBuffer( s_lines, sizeof( s_lines ) / sizeof( glm::vec3 ) );

	RayRenderer::RayRenderer() :
		Renderer( "Ray" )
	{
		RequireTag( ddc::Tag::Visible );
		Require<dd::RayComponent>();
	}

	RayRenderer::~RayRenderer()
	{

	}

	void RayRenderer::RenderInit( ddc::World& world )
	{
		m_shader = ShaderProgram::Load( "line" );
		DD_ASSERT( m_shader.Valid() );

		ShaderProgram* shader = ShaderProgram::Get( m_shader );
		DD_ASSERT( shader != nullptr );

		shader->Use( true );

		m_vao.Create();
		m_vao.Bind();

		m_vbo.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vbo.Bind();
		m_vbo.SetData( s_linesBuffer );

		shader->BindPositions();

		m_vbo.Unbind();
		m_vao.Unbind();

		shader->Use( false );
	}

	glm::mat4 TransformFromRay( const dd::Ray& ray )
	{
		glm::vec3 dir = ray.Direction();

		glm::vec3 right = glm::normalize( glm::cross( dir, glm::vec3( 0, 1, 0 ) ) );
		glm::vec3 up = glm::normalize( glm::cross( dir, right ) );

		return glm::mat4(
			glm::vec4( right, 0 ),
			glm::vec4( up, 0 ),
			glm::vec4( dir, 0 ),
			glm::vec4( ray.Origin(), 1 ) );
	}

	void RayRenderer::Render( const ddr::RenderData& data )
	{
		ShaderProgram* shader = ShaderProgram::Get( m_shader );
		DD_ASSERT( shader != nullptr );
		
		shader->Use( true );

		m_vao.Bind();
		m_vbo.Bind();

		glm::mat4 view_projection = data.Camera().GetProjectionMatrix() * data.Camera().GetViewMatrix();

		shader->SetUniform( "Colour", glm::vec4( 1, 1, 0, 1 ) );

		for( const dd::RayComponent& ray : data.Get<dd::RayComponent>() )
		{
			float scale = ray.Length > 0 ? ray.Length : 100;
			glm::mat4 model = TransformFromRay( ray.Ray ) * glm::scale( glm::vec3( scale ) );

			shader->SetUniform( "ModelViewProjection", view_projection * model );

			glDrawArrays( GL_LINES, 0, m_vbo.GetDataSize() );
		}

		m_vbo.Unbind();
		m_vao.Unbind();

		shader->Use( false );
	}
}