//
// Mesh.cpp - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#include "PrecompiledHeader.h"
#include "Mesh.h"

#include "Camera.h"
#include "Shader.h"

#include "GL/gl3w.h"

#include "glm/gtx/transform.hpp"

namespace dd
{
	std::mutex Mesh::m_instanceMutex;
	std::unordered_map<uint64, Mesh> Mesh::m_instances;

	float s_unitCube[] = 
	{
		//  X    Y    Z     U     V       Normal				
		// bottom
		-1.0f,-1.0f,-1.0f,  0.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,  0.0f, 1.0f,   0.0f, -1.0f, 0.0f,
		1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		1.0f,-1.0f, 1.0f,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,  0.0f, 1.0f,   0.0f, -1.0f, 0.0f,

		// top
		-1.0f, 1.0f,-1.0f,  0.0f, 0.0f,   0.0f, 1.0f, 0.0f,	
		-1.0f, 1.0f, 1.0f,  0.0f, 1.0f,   0.0f, 1.0f, 0.0f,	
		1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,	
		1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,	
		-1.0f, 1.0f, 1.0f,  0.0f, 1.0f,   0.0f, 1.0f, 0.0f,	
		1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f,	

		// front
		-1.0f,-1.0f, 1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,	
		1.0f,-1.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,	
		-1.0f, 1.0f, 1.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f,	
		1.0f,-1.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,	
		1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,	
		-1.0f, 1.0f, 1.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f,	

		// back
		-1.0f,-1.0f,-1.0f,  0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f,-1.0f,  0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f,-1.0f,  0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		1.0f, 1.0f,-1.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,

		// left
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f,-1.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		-1.0f,-1.0f,-1.0f,   0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,   1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f,-1.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

		// right
		1.0f,-1.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,	
		1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,	
		1.0f, 1.0f,-1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,	
		1.0f,-1.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,	
		1.0f, 1.0f,-1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,	
		1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f
	};

	Mesh* Mesh::Get( MeshHandle handle )
	{
		std::lock_guard<std::mutex> lock( m_instanceMutex );

		auto it = m_instances.find( handle.m_hash );
		if( it == m_instances.end() )
		{
			return nullptr;
		}

		return &it->second;
	}

	MeshHandle Mesh::Create( const char* name, ShaderHandle program )
	{
		DD_ASSERT( name != nullptr );
		DD_ASSERT( strlen( name ) > 0 );

		uint64 hash = dd::HashString( name, strlen( name ) );
		hash ^= program.m_hash;

		std::lock_guard<std::mutex> lock( m_instanceMutex );

		auto it = m_instances.find( hash );
		if( it == m_instances.end() )
		{
			m_instances.insert( std::make_pair( hash, Mesh( name, program ) ) );
		}

		MeshHandle handle;
		handle.m_hash = hash;

		return handle;
	}

	void Mesh::Destroy( MeshHandle handle )
	{
		std::lock_guard<std::mutex> lock( m_instanceMutex );

		m_instances.erase( handle.m_hash );
	}

	Mesh::Mesh( const char* name, ShaderHandle program ) :
		m_refCount( nullptr ),
		m_vbo( OpenGL::InvalidID ),
		m_indexVBO( OpenGL::InvalidID ),
		m_shader( program ),
		m_name( name ),
		m_stride( 0 )
	{
		DD_PROFILE_SCOPED( Mesh_Create );

		m_vao.Create();

		glGenBuffers( 1, &m_vbo );

		m_refCount = new std::atomic<int>( 1 );
	}

	Mesh::Mesh( const Mesh& other ) :
		m_refCount( other.m_refCount ),
		m_vao( other.m_vao ),
		m_vbo( other.m_vbo ),
		m_name( other.m_name ),
		m_shader( other.m_shader ),
		m_bounds( other.m_bounds ),
		m_colour( other.m_colour )
	{
		Retain();
	}

	Mesh::~Mesh()
	{
		Release();
	}

	Mesh& Mesh::operator=( const Mesh& other )
	{
		Release();

		m_refCount = other.m_refCount;
		m_vao = other.m_vao;
		m_vbo = other.m_vbo;
		m_name = other.m_name;
		m_shader = other.m_shader;

		Retain();

		return *this;
	}

	void Mesh::SetData( float* data, int count, int stride )
	{
		m_data.Set( data, count );
		m_stride = stride;

		m_vao.Bind();

		glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
		glBufferData( GL_ARRAY_BUFFER, count, data, GL_STATIC_DRAW );

		m_vao.Unbind();
	}

	void Mesh::UpdateData()
	{
		m_vao.Bind();

		glBufferData( GL_ARRAY_BUFFER, m_data.Size(), m_data.Get(), GL_STATIC_DRAW );

		m_vao.Unbind();
	}

	void Mesh::SetIndices( uint* data, int count )
	{
		m_indices.Set( data, count );
		
		m_vao.Bind();

		glGenBuffers( 1, &m_indexVBO );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indexVBO );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, count * sizeof( uint ), data, GL_STATIC_DRAW );

		m_vao.Unbind();
	}

	void Mesh::BindAttribute( const char* shaderAttribute, uint components, uint first, bool normalized )
	{
		DD_ASSERT( m_data.Get() != nullptr );
		DD_ASSERT( m_stride > 0 );

		m_vao.Bind();

		m_shader.Get()->BindAttributeFloat( shaderAttribute, components, m_stride, first, normalized );

		m_vao.Unbind();
	}

	const AABB& Mesh::Bounds() const
	{
		return m_bounds;
	}

	void Mesh::SetBounds( const AABB& bounds )
	{
		m_bounds = bounds;
	}

	void Mesh::Render( const Camera& camera, const glm::mat4& transform )
	{
		DD_PROFILE_SCOPED( Mesh_Render );

		ShaderProgram& shader = *m_shader.Get();
		DD_ASSERT( shader.IsValid() );

		shader.Use( true );

		glm::mat4 view = camera.GetCameraMatrix();

		shader.SetUniform( "Model", transform );
		shader.SetUniform( "View", view );
		shader.SetUniform( "Projection", camera.GetProjection() );
		shader.SetUniform( "NormalMatrix", glm::transpose( glm::inverse( glm::mat3( transform ) ) ) );
		shader.SetUniform( "ObjectColour", m_colour );

		m_vao.Bind();

		if( m_indices.Get() == nullptr )
		{
			glDrawArrays( GL_TRIANGLES, 0, 6 * 2 * 3 );
		}
		else
		{
			int size;  
			glGetBufferParameteriv( GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size );
			glDrawElements( GL_TRIANGLES, size / sizeof(uint), GL_UNSIGNED_INT, 0 );
		}

		m_vao.Unbind();

		shader.Use( false );
	}

	void Mesh::Retain()
	{ 
		DD_ASSERT( m_refCount != nullptr );

		++*m_refCount;
	}

	void Mesh::Release()
	{
		DD_ASSERT( m_refCount != nullptr );

		m_data.Release();

		if( --*m_refCount <= 0 )
		{
			glDeleteBuffers( 1, &m_vbo );

			if( m_indices.Get() != nullptr )
			{
				glDeleteBuffers( 1, &m_indexVBO );
			}
			
			m_vao.Destroy();

			delete m_refCount;

			m_vbo = OpenGL::InvalidID;
			m_indexVBO = OpenGL::InvalidID;
		}
	}

	void Mesh::MakeUnitCube()
	{
		SetData( s_unitCube, sizeof( s_unitCube ), 8 );
		
		ShaderProgram& shader = *m_shader.Get();
		shader.Use( true );

		BindAttribute( "Position", 3, 0, false );
		BindAttribute( "UV", 2, 3, false );
		BindAttribute( "Normal", 3, 5, true );

		shader.Use( false );

		AABB bounds;
		bounds.Expand( glm::vec3( -1, -1, -1 ) );
		bounds.Expand( glm::vec3( 1, 1, 1 ) );
		SetBounds( bounds );
	}
}