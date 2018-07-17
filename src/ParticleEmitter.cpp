//
// ParticleEmitter.cpp - A particle emitter.
// Copyright (C) Sebastian Nordgren 
// July 16th 2018
//

#include "PrecompiledHeader.h"
#include "ParticleEmitter.h"

#include "OpenGL.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Uniforms.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "GL/gl3w.h"

namespace ddr
{
	static const glm::vec3 s_screenFacingQuadVertices[] = {
		glm::vec3( -1.0f, -1.0f, 0.0f ),
		glm::vec3( 1.0f, -1.0f, 0.0f ),
		glm::vec3( -1.0f,  1.0f, 0.0f ),
		glm::vec3( -1.0f,  1.0f, 0.0f ),
		glm::vec3( 1.0f, -1.0f, 0.0f ),
		glm::vec3( 1.0f,  1.0f, 0.0f )
	};

	static VAO s_vaoParticle;
	static VBO s_vboParticle;
	static ShaderHandle s_shaderParticle;

	static const int MaxParticles = 512;

	static glm::vec3 s_positions[ MaxParticles ];
	static VBO s_vboPositions;

	static glm::vec3 s_sizes[ MaxParticles ];
	static VBO s_vboSizes;

	static glm::vec4 s_colours[ MaxParticles ];
	static VBO s_vboColours;

	ParticleEmitter::ParticleEmitter()
	{

	}

	ParticleEmitter::~ParticleEmitter()
	{

	}

	void ParticleEmitter::CreateRenderResources()
	{
		dd::Vector<Shader*> shaders;

		Shader* vert = Shader::Create( dd::String32( "shaders\\particle.vertex" ), Shader::Type::Vertex );
		DD_ASSERT( vert != nullptr );
		shaders.Add( vert );

		Shader* pixel = Shader::Create( dd::String32( "shaders\\particle.pixel" ), Shader::Type::Pixel );
		DD_ASSERT( pixel != nullptr );
		shaders.Add( pixel );

		s_shaderParticle = ShaderProgram::Create( dd::String8( "particle" ), shaders );

		ShaderProgram* shader = ShaderProgram::Get( s_shaderParticle );
		DD_ASSERT( shader != nullptr );

		shader->Use( true );

		s_vaoParticle.Create();
		s_vaoParticle.Bind();

		s_vboParticle.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		s_vboParticle.Bind();
		s_vboParticle.SetData( dd::ConstBuffer<glm::vec3>( s_screenFacingQuadVertices, 6 ) );
		shader->BindPositions();
		s_vboParticle.Unbind();

		s_vboPositions.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		s_vboPositions.Bind();
		s_vboPositions.SetData( dd::ConstBuffer<glm::vec3>( s_positions, MaxParticles ) );
		shader->BindAttributeVec3( "PositionInstanced", false );
		shader->SetAttributeInstanced( "PositionInstanced" );
		s_vboPositions.Unbind();

		s_vboSizes.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		s_vboSizes.Bind();
		s_vboSizes.SetData( dd::ConstBuffer<glm::vec3>( s_sizes, MaxParticles ) );
		shader->BindAttributeVec3( "ScaleInstanced", false );
		shader->SetAttributeInstanced( "ScaleInstanced" );
		s_vboSizes.Unbind();

		s_vboColours.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		s_vboColours.Bind();
		s_vboColours.SetData( dd::ConstBuffer<glm::vec4>( s_colours, MaxParticles ) );
		shader->BindAttributeVec3( "ColourInstanced", false );
		shader->SetAttributeInstanced( "ColourInstanced" );
		s_vboColours.Unbind();

		s_vaoParticle.Unbind();

		shader->Use( false );
	}

	void ParticleEmitter::Update( float delta_t )
	{
		m_age += delta_t;

		// update existing
		for( Particle& particle : m_particles )
		{
			if( particle.Alive )
			{
				particle.Age += delta_t;

				if( particle.Age > particle.Lifetime )
				{
					particle.Alive = false;
					--m_liveCount;
					continue;
				}

				float percent_life = particle.Age / particle.Lifetime;
				particle.Colour = glm::vec4( percent_life );

				particle.Size = glm::vec2( 1.0f * percent_life );

				particle.Position += particle.Velocity * delta_t;
			}
		}
	}

	void ParticleEmitter::Render( const dd::ICamera& camera, UniformStorage& uniforms )
	{
		for( int i = 0; i < 10; ++i )
		{
			s_positions[ i ] = glm::vec3( (float) i );
			s_sizes[ i ] = glm::vec3( (float) i );
			s_colours[ i ] = glm::vec4( 1 );
		}

		s_vboPositions.UpdateData();
		s_vboSizes.UpdateData();
		s_vboColours.UpdateData();
		
		s_vaoParticle.Bind();
	}
}