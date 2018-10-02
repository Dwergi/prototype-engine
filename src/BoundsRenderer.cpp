//
// BoundsRenderer.cpp - A debug renderer for bounds.
// Copyright (C) Sebastian Nordgren 
// September 13th 2018
//

#include "PCH.h"
#include "BoundsRenderer.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "BoundsHelpers.h"
#include "InputBindings.h"
#include "MeshUtils.h"
#include "OpenGL.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TransformComponent.h"

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

	static dd::ConstBuffer<glm::vec3> s_cornersBuffer( s_corners, dd::ArrayLength( s_corners ) );

	static const uint s_indices[] = 
	{
		// bottom
		0,1,	1,3,	3,2,	2,0,
		// top
		4,5,	5,7,	7,6,	6,4,
		// corners
		0,4,	1,5,	2,6,	3,7
	};

	static dd::ConstBuffer<uint> s_indicesBuffer( s_indices, dd::ArrayLength( s_indices ) );

	BoundsRenderer::BoundsRenderer() :
		Renderer( "Bounds" )
	{
		Require<dd::TransformComponent>();
		Optional<dd::BoundBoxComponent>();
		Optional<dd::BoundSphereComponent>();
		RequireTag( ddc::Tag::Visible );
	}

	BoundsRenderer::~BoundsRenderer()
	{

	}

	void BoundsRenderer::BindKeys( dd::InputBindings& bindings )
	{
		auto handler = [this]( dd::InputAction action, dd::InputType type )
		{
			if( type == dd::InputType::RELEASED )
			{
				int mode = (int) m_drawMode;
				++mode;

				if( mode > 2 )
					mode = 0;

				m_drawMode = (DrawMode) mode;
				m_updateBuffers = true;
			}
		};

		bindings.RegisterHandler( dd::InputAction::TOGGLE_BOUNDS, handler );
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

		m_vboIndex.Create( GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW );

		m_vao.Unbind();

		shader->Use( false );

		m_updateBuffers = true;
	}

	void BoundsRenderer::UpdateBuffers()
	{
		if( m_drawMode == DrawMode::Box )
		{
			m_vboPosition.Bind();
			m_vboPosition.SetData( s_cornersBuffer );
			m_vboPosition.CommitData();
			m_vboPosition.Unbind();

			m_vboIndex.Bind();
			m_vboIndex.SetData( s_indicesBuffer );
			m_vboIndex.CommitData();
			m_vboIndex.Unbind();
		}
		else if( m_drawMode == DrawMode::Sphere )
		{
			dd::MakeIcosphereLines( m_vboPosition, m_vboIndex, m_subdivisions );
		}

		m_updateBuffers = false;
	}

	void BoundsRenderer::Render( const ddr::RenderData& data )
	{
		if( m_drawMode == DrawMode::None )
		{
			return;
		}

		if( m_updateBuffers )
		{
			UpdateBuffers();
		}

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

		auto transforms = data.Get<dd::TransformComponent>();
		auto bound_boxes = data.Get<dd::BoundBoxComponent>();
		auto bound_spheres = data.Get<dd::BoundSphereComponent>();

		auto entities = data.Entities();

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

			dd::AABB bound_box;
			dd::Sphere bound_sphere;
			if( !GetWorldBoundBoxAndSphere( bound_boxes.Get( i ), bound_spheres.Get( i ), transforms[i].Transform, bound_box, bound_sphere ) )
			{
				continue;
			}

			if( m_drawMode == DrawMode::Box )
			{
				glm::mat4 model = glm::translate( bound_box.Min ) * glm::scale( bound_box.Max - bound_box.Min );

				shader->SetUniform( "ModelViewProjection", view_projection * model );
			}
			else if( m_drawMode == DrawMode::Sphere )
			{
				glm::mat4 model = glm::translate( bound_sphere.Centre ) * glm::scale( glm::vec3( bound_sphere.Radius ) );

				shader->SetUniform( "ModelViewProjection", view_projection * model );
			}

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
		ImGui::SetWindowSize( ImVec2( 200, 120 ), ImGuiCond_Once );

		static const char* c_drawModes = "None\0Box\0Sphere\0";
		
		int drawMode = (int) m_drawMode;
		if( ImGui::Combo( "Mode", &drawMode, c_drawModes ) )
		{
			m_drawMode = (DrawMode) drawMode;
			m_updateBuffers = true;
		}

		if( ImGui::SliderInt( "Subdivisions", &m_subdivisions, 0, 6 ) )
		{
			m_updateBuffers = true;
		}

		int triangles = (m_vboIndex.GetDataSize() / sizeof(uint)) / 6;

		ImGui::Value( "Triangles", triangles );
	}
}