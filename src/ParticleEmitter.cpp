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

	static const dd::ConstBuffer<glm::vec3> s_screenFacingQuadBuffer( s_screenFacingQuadVertices, 6 );

	static VAO s_vaoParticle;
	static VBO s_vboParticle;
	static ShaderHandle s_shaderParticle;

	ParticleEmitter::ParticleEmitter()
	{

	}

	ParticleEmitter::~ParticleEmitter()
	{

	}

	void ParticleEmitter::CreateRenderResources()
	{
		s_vaoParticle.Create();
		s_vaoParticle.Bind();

		s_vboParticle.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		s_vboParticle.Bind();
		s_vboParticle.SetData( s_screenFacingQuadBuffer );

		s_vaoParticle.Unbind();

		dd::Vector<Shader*> shaders;

		Shader* vert = Shader::Create( dd::String32( "shaders\\particle.vertex" ), Shader::Type::Vertex );
		DD_ASSERT( vert != nullptr );
		shaders.Add( vert );

		Shader* pixel = Shader::Create( dd::String32( "shaders\\particle.pixel" ), Shader::Type::Pixel );
		DD_ASSERT( pixel != nullptr );
		shaders.Add( pixel );

		s_shaderParticle = ShaderProgram::Create( dd::String8( "particle" ), shaders );
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
		static glm::mat4 s_transforms[ 10 ];
		static glm::vec4 s_colours[ 10 ];

		for( int i = 0; i < 10; ++i )
		{
			s_transforms[ i ] = glm::scale( glm::translate( glm::vec3( (float) i ) ), glm::vec3( 0.05f ) );
			s_colours[ i ] = glm::vec4( 1 );
		}
		
		s_vaoParticle.Bind();

	}
}