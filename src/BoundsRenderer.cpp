#include "PrecompiledHeader.h"
#include "BoundsRenderer.h"

#include "BoundsComponent.h"
#include "OpenGL.h"
#include "Shader.h"
#include "ShaderProgram.h"

#include "imgui/imgui.h"

namespace ddr
{
	static const glm::vec3 s_corners[] =
	{
		// bottom
		glm::vec3( 0, 0, 0 ),
		glm::vec3( 1, 0, 0 ),
		glm::vec3( 0, 0, 1 ),
		glm::vec3( 1, 0, 1 ),

		// top
		glm::vec3( 0, 1, 0 ),
		glm::vec3( 1, 1, 0 ),
		glm::vec3( 0, 1, 1 ),
		glm::vec3( 1, 1, 1 )
	};

	static dd::ConstBuffer<glm::vec3> s_cornersBuffer( s_corners, sizeof( s_corners ) / sizeof( glm::vec3 ) );

	static const uint s_indices[] = 
	{
		// bottom
		0,1,	1,3,	3,2,	2,0,

		// top
		4,5,	5,7,	7,6,	6,4,

		// corners
		0,4,	1,5,	2,6,	3,7
	};

	static dd::ConstBuffer<uint> s_indicesBuffer( s_indices, sizeof( s_indices ) / sizeof( uint ) );

	BoundsRenderer::BoundsRenderer() :
		Renderer( "Bounds" )
	{
		Require<dd::BoundsComponent>();
		RequireTag( ddc::Tag::Visible );
	}

	BoundsRenderer::~BoundsRenderer()
	{

	}
	
	void BoundsRenderer::RenderInit( ddc::World& world )
	{
		m_shader = ShaderProgram::Load( "line" );
		DD_ASSERT( m_shader.Valid() );

		ShaderProgram* shader = ShaderProgram::Get( m_shader );
		DD_ASSERT( shader != nullptr );

		shader->Use( true );

		m_vao.Create();
		m_vao.Bind();

		m_vboPosition.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboPosition.Bind();
		shader->BindPositions();
		m_vboPosition.Unbind();

		m_vboPosition.SetData( s_cornersBuffer );

		m_vboIndex.Create( GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboIndex.Bind();
		m_vboIndex.SetData( s_indicesBuffer );
		m_vboIndex.Unbind();

		m_vao.Unbind();

		shader->Use( false );
	}

	void BoundsRenderer::Render( const ddr::RenderData& data )
	{
		if( !m_draw )
			return;

		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		ShaderProgram* shader = ShaderProgram::Get( m_shader );
		DD_ASSERT( shader != nullptr );

		shader->Use( true );

		m_vao.Bind();

		m_vboPosition.Bind();
		m_vboIndex.Bind();

		const ddc::World& world = data.World();

		glm::mat4 view_projection = data.Camera().GetProjectionMatrix() * data.Camera().GetViewMatrix();

		ddr::RenderBuffer<dd::BoundsComponent> bounds = data.Get<dd::BoundsComponent>();
		const dd::Span<ddc::Entity>& entities = data.Entities();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			if( world.HasTag( entities[i], ddc::Tag::Selected ) )
			{
				shader->SetUniform( "Colour", glm::vec4( 0, 1, 0, 1 ) );
			}
			else if( world.HasTag( entities[i], ddc::Tag::Focused ) )
			{
				shader->SetUniform( "Colour", glm::vec4( 1, 1, 0, 1 ) );
			}
			else
			{
				shader->SetUniform( "Colour", glm::vec4( 1, 1, 1, 0.5 ) );
			}

			const dd::BoundsComponent& bb = bounds[i];

			glm::mat4 model = glm::translate( bb.World.Min ) * glm::scale( bb.World.Max - bb.World.Min );

			shader->SetUniform( "ModelViewProjection", view_projection * model );

			glDrawElements( GL_LINES, m_vboIndex.GetDataSize(), GL_UNSIGNED_INT, 0 );
		}

		m_vboIndex.Unbind();
		m_vboPosition.Unbind();

		m_vao.Unbind();

		shader->Use( false );

		glDisable( GL_BLEND );
	}

	void BoundsRenderer::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::Checkbox( "Draw", &m_draw );
	}
}