//
// ParticleEmitter.cpp - A particle emitter.
// Copyright (C) Sebastian Nordgren 
// July 16th 2018
//

#include "PrecompiledHeader.h"
#include "ParticleEmitter.h"

#include "ICamera.h"
#include "OpenGL.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Uniforms.h"

#include "glm/gtx/norm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

#include "GL/gl3w.h"

#include "imgui/imgui.h"

#include <algorithm>

namespace ddr
{
	static const glm::vec2 s_screenFacingQuadVertices[] = {
		glm::vec2( -0.5f,	-0.5f	),
		glm::vec2( 0.5f,	-0.5f	),
		glm::vec2( -0.5f,	0.5f	),
		glm::vec2( -0.5f,	0.5f	),
		glm::vec2( 0.5f,	-0.5f	),
		glm::vec2( 0.5f,	0.5f	)
	};

	static const glm::vec2 s_screenFacingQuadUVs[] = {
		glm::vec2( 0,	0 ),
		glm::vec2( 1,	0 ),
		glm::vec2( 0,	1 ),
		glm::vec2( 0,	1 ),
		glm::vec2( 1,	0 ),
		glm::vec2( 1,	1 )
	};

	static VAO s_vaoParticle;
	static VBO s_vboParticle;
	static ShaderHandle s_shaderParticle;

	static const glm::vec3 s_gravity( 0, -9.81, 0 );

	dd::RandomFloat ParticleEmitter::s_rngLifetime( 0.5f, 5.0f );

	dd::RandomFloat ParticleEmitter::s_rngSizeX( 0, 1 );
	dd::RandomFloat ParticleEmitter::s_rngSizeY( 0, 1 );

	dd::RandomFloat ParticleEmitter::s_rngVelocityX( -10.0f, 10.0f );
	dd::RandomFloat ParticleEmitter::s_rngVelocityY( -10.0f, 10.0f );
	dd::RandomFloat ParticleEmitter::s_rngVelocityZ( -10.0f, 10.0f );

	dd::RandomFloat ParticleEmitter::s_rngColourR( 0, 1 );
	dd::RandomFloat ParticleEmitter::s_rngColourG( 0, 1 );
	dd::RandomFloat ParticleEmitter::s_rngColourB( 0, 1 );

	int ParticleEmitter::CurrentParticles = 1000;

	ParticleEmitter::ParticleEmitter()
	{

	}

	ParticleEmitter::~ParticleEmitter()
	{

	}

	void ParticleEmitter::RenderInit()
	{
		dd::Vector<Shader*> shaders;

		if( !s_shaderParticle.Valid() )
		{
			s_shaderParticle = ShaderProgram::Load( "particle" );
		}

		ShaderProgram* shader = ShaderProgram::Get( s_shaderParticle );
		DD_ASSERT( shader != nullptr );

		shader->Use( true );

		s_vaoParticle.Create();
		s_vaoParticle.Bind();

		s_vboParticle.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		s_vboParticle.Bind();
		s_vboParticle.SetData( dd::ConstBuffer<glm::vec2>( s_screenFacingQuadVertices, 6 ) );
		shader->BindAttributeVec2( "Position", false );
		s_vboParticle.Unbind();

		m_vboPositions.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboPositions.Bind();
		m_vboPositions.SetData( dd::ConstBuffer<glm::vec3>( m_positions, MaxParticles ) );
		shader->BindAttributeVec3( "PositionInstanced", false );
		shader->SetAttributeInstanced( "PositionInstanced" );
		m_vboPositions.Unbind();

		m_vboSizes.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboSizes.Bind();
		m_vboSizes.SetData( dd::ConstBuffer<glm::vec2>( m_sizes, MaxParticles ) );
		shader->BindAttributeVec2( "ScaleInstanced", false );
		shader->SetAttributeInstanced( "ScaleInstanced" );
		m_vboSizes.Unbind();

		m_vboColours.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboColours.Bind();
		m_vboColours.SetData( dd::ConstBuffer<glm::vec4>( m_colours, MaxParticles ) );
		shader->BindAttributeVec4( "ColourInstanced", false );
		shader->SetAttributeInstanced( "ColourInstanced" );
		m_vboColours.Unbind();

		s_vaoParticle.Unbind();

		shader->Use( false );
	}

	void ParticleEmitter::Update( dd::EntityManager& entity_manager, float delta_t )
	{
		m_age += delta_t;

		// update existing
		for( Particle& particle : m_particles )
		{
			if( particle.Alive() )
			{
				particle.Age += delta_t;

				if( !particle.Alive() )
				{
					--m_liveCount;
					continue;
				}

				float percent_life = particle.Age / particle.Lifetime;
				particle.Colour.a = 1.0f - percent_life;

				particle.Position += particle.Velocity * delta_t;
				particle.Velocity += s_gravity * delta_t;
			}
		}

		for( int i = 0; i < CurrentParticles; ++i )
		{
			Particle& particle = m_particles[ i ];

			if( !particle.Alive() )
			{
				particle.Position = glm::vec3( 0, 50, 0 );
				particle.Velocity = glm::vec3( s_rngVelocityX.Next(), s_rngVelocityY.Next(), s_rngVelocityZ.Next() );
				particle.Size = glm::vec2( s_rngSizeX.Next(), s_rngSizeY.Next() );
				particle.Lifetime = s_rngLifetime.Next();
				particle.Age = 0;
				particle.Colour = glm::vec4( s_rngColourR.Next(), s_rngColourG.Next(), s_rngColourB.Next(), 1 );

				++m_liveCount;
			}
		}
	}

	void ParticleEmitter::Render( const dd::EntityManager& entity_manager, const dd::ICamera& camera, UniformStorage& uniforms )
	{
		for( Particle& particle : m_particles )
		{
			if( particle.Alive() )
			{
				particle.Distance = glm::distance2( particle.Position, camera.GetPosition() );
			}
			else
			{
				particle.Distance = -1;
			}
		}

		std::sort( &m_particles[ 0 ], &m_particles[ MaxParticles ], []( const Particle& a, const Particle& b )
		{
			return a.Distance > b.Distance;
		} );

		int index = 0;
		for( Particle& particle : m_particles )
		{
			if( particle.Alive() )
			{
				m_positions[ index ] = particle.Position;
				m_colours[ index ] = particle.Colour;
				m_sizes[ index ] = particle.Size;

				++index;
			}
		}

		m_vboPositions.UpdateData();
		m_vboSizes.UpdateData();
		m_vboColours.UpdateData();
		
		s_vaoParticle.Bind();

		ShaderProgram* shader = ShaderProgram::Get( s_shaderParticle );
		shader->Use( true );

		uniforms.Bind( *shader );

		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glDrawArraysInstanced( GL_TRIANGLES, 0, 6, index );

		glDisable( GL_BLEND );

		shader->Use( false );

		s_vaoParticle.Unbind();
	}

	void ParticleEmitter::DrawDebugInternal()
	{
		ImGui::SliderInt( "Max Particles", &CurrentParticles, 0, MaxParticles );

		{
			float lifetime_min = s_rngLifetime.Min();
			float lifetime_max = s_rngLifetime.Max();

			if( ImGui::SliderFloat( "Lifetime Min", &lifetime_min, 0, lifetime_max ) )
			{
				s_rngLifetime.SetMin( lifetime_min );
			}

			if( ImGui::SliderFloat( "Lifetime Max", &lifetime_max, lifetime_min, 10 ) )
			{
				s_rngLifetime.SetMax( lifetime_max );
			}
		}

		if( ImGui::TreeNodeEx( "Colour", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			{
				float colour_r_min = s_rngColourR.Min();
				float colour_r_max = s_rngColourR.Max();
				if( ImGui::DragFloatRange2( "R", &colour_r_min, &colour_r_max, 0.001f, 0, 1 ) )
				{
					s_rngColourR.SetMin( colour_r_min );
					s_rngColourR.SetMax( colour_r_max );
				}

			}

			{
				float colour_g_min = s_rngColourG.Min();
				float colour_g_max = s_rngColourG.Max();
				if( ImGui::DragFloatRange2( "G", &colour_g_min, &colour_g_max, 0.001f, 0, 1 ) )
				{
					s_rngColourG.SetMin( colour_g_min );
					s_rngColourG.SetMax( colour_g_max );
				}
			}

			{
				float colour_b_min = s_rngColourB.Min();
				float colour_b_max = s_rngColourB.Max();
				if( ImGui::DragFloatRange2( "B", &colour_b_min, &colour_b_max, 0.001f, 0, 1 ) )
				{
					s_rngColourB.SetMin( colour_b_min );
					s_rngColourB.SetMax( colour_b_max );
				}
			}

			ImGui::TreePop();
		}
		
		if( ImGui::TreeNodeEx( "Velocity", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			{
				float velocity_x_min = s_rngVelocityX.Min();
				float velocity_x_max = s_rngVelocityX.Max();
				if( ImGui::DragFloatRange2( "X", &velocity_x_min, &velocity_x_max, 0.1f, -50, 50, "%.1f" ) )
				{
					s_rngVelocityX.SetMin( velocity_x_min );
					s_rngVelocityX.SetMax( velocity_x_max );
				}
			}

			{
				float velocity_y_min = s_rngVelocityY.Min();
				float velocity_y_max = s_rngVelocityY.Max();
				if( ImGui::DragFloatRange2( "Y", &velocity_y_min, &velocity_y_max, 0.1f, -50, 50, "%.1f" ) )
				{
					s_rngVelocityY.SetMin( velocity_y_min );
					s_rngVelocityY.SetMax( velocity_y_max );
				}
			}

			{
				float velocity_z_min = s_rngVelocityZ.Min();
				float velocity_z_max = s_rngVelocityZ.Max();
				if( ImGui::DragFloatRange2( "Z", &velocity_z_min, &velocity_z_max, 0.1f, -50, 50, "%.1f" ) )
				{
					s_rngVelocityZ.SetMin( velocity_z_min );
					s_rngVelocityZ.SetMax( velocity_z_max );
				}
			}

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Size", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			{
				float size_x_min = s_rngSizeX.Min();
				float size_x_max = s_rngSizeX.Max();
				if( ImGui::DragFloatRange2( "X", &size_x_min, &size_x_max, 0.01f, 0, 5, "%.2f" ) )
				{
					s_rngSizeX.SetMin( size_x_min );
					s_rngSizeX.SetMax( size_x_max );
				}
			}

			{
				float size_y_min = s_rngSizeY.Min();
				float size_y_max = s_rngSizeY.Max();
				if( ImGui::DragFloatRange2( "Y", &size_y_min, &size_y_max, 0.01f, 0, 5, "%.2f" ) )
				{
					s_rngSizeY.SetMin( size_y_min );
					s_rngSizeY.SetMax( size_y_max );
				}
			}

			ImGui::TreePop();
		}
	}
}