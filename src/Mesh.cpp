//
// Mesh.cpp - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#include "PrecompiledHeader.h"
#include "Mesh.h"

#include "ICamera.h"
#include "GLError.h"
#include "Shader.h"

#include "GL/gl3w.h"

#include "glm/gtx/transform.hpp"

namespace dd
{
	std::mutex Mesh::m_instanceMutex;
	std::unordered_map<uint64, Mesh*> Mesh::m_instances;

	static const glm::vec3 s_unitCubePositions[] =
	{
		//  X    Y    Z     
		// bottom
		glm::vec3( -1.0f,	-1.0f,	-1.0f ),
		glm::vec3( 1.0f,	-1.0f,	-1.0f ), 
		glm::vec3( -1.0f,	-1.0f,	1.0f ),
		glm::vec3( 1.0f,	-1.0f,	-1.0f ),
		glm::vec3( 1.0f,	-1.0f,	1.0f ),
		glm::vec3( -1.0f,	-1.0f,	1.0f ),
				
		// top			 
		glm::vec3( -1.0f,	1.0f,	-1.0f ),
		glm::vec3( -1.0f,	1.0f,	1.0f ),
		glm::vec3( 1.0f,	1.0f,	-1.0f ),
		glm::vec3( 1.0f,	1.0f,	-1.0f ),
		glm::vec3( -1.0f,	1.0f,	1.0f ),
		glm::vec3( 1.0f,	1.0f,	1.0f ),
		
		// front
		glm::vec3( -1.0f,	-1.0f,	1.0f ),
		glm::vec3( 1.0f,	-1.0f,	1.0f ),
		glm::vec3( -1.0f,	1.0f,	1.0f ),
		glm::vec3( 1.0f,	-1.0f,	1.0f ),
		glm::vec3( 1.0f,	1.0f,	1.0f ),
		glm::vec3( -1.0f,	1.0f,	1.0f ),
		
		// back
		glm::vec3( -1.0f,	-1.0f,	-1.0f ),
		glm::vec3( -1.0f,	1.0f,	-1.0f ),
		glm::vec3( 1.0f,	-1.0f,	-1.0f ),
		glm::vec3( 1.0f,	-1.0f,	-1.0f ),
		glm::vec3( -1.0f,	1.0f,	-1.0f ),
		glm::vec3( 1.0f,	1.0f,	-1.0f ),

		// left
		glm::vec3( -1.0f,	-1.0f,	1.0f ),
		glm::vec3( -1.0f,	1.0f,	-1.0f ),
		glm::vec3( -1.0f,	-1.0f,	-1.0f ),
		glm::vec3( -1.0f,	-1.0f,	1.0f ),
		glm::vec3( -1.0f,	1.0f,	1.0f ),
		glm::vec3( -1.0f,	1.0f,	-1.0f ),

		// right
		glm::vec3( 1.0f,	-1.0f,	1.0f ),
		glm::vec3( 1.0f,	-1.0f,	-1.0f ),
		glm::vec3( 1.0f,	1.0f,	-1.0f ),
		glm::vec3( 1.0f,	-1.0f,	1.0f ),
		glm::vec3( 1.0f,	1.0f,	-1.0f ),
		glm::vec3( 1.0f,	1.0f,	1.0f ),
	};

	static ConstBuffer<glm::vec3> s_unitCubePositionsBuffer( s_unitCubePositions, sizeof( s_unitCubePositions ) / sizeof( glm::vec3 ) );

	static const glm::vec3 s_unitCubeNormals[] =
	{
		// bottom
		glm::vec3( 0.0f, -1.0f, 0.0f ),
		glm::vec3( 0.0f, -1.0f, 0.0f ),
		glm::vec3( 0.0f, -1.0f, 0.0f ),
		glm::vec3( 0.0f, -1.0f, 0.0f ),
		glm::vec3( 0.0f, -1.0f, 0.0f ),
		glm::vec3( 0.0f, -1.0f, 0.0f ),

		// top
		glm::vec3( 0.0f, 1.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ),

		// front
		glm::vec3( 0.0f, 0.0f, 1.0f ),
		glm::vec3( 0.0f, 0.0f, 1.0f ),
		glm::vec3( 0.0f, 0.0f, 1.0f ),
		glm::vec3( 0.0f, 0.0f, 1.0f ),
		glm::vec3( 0.0f, 0.0f, 1.0f ),
		glm::vec3( 0.0f, 0.0f, 1.0f ),

		// back
		glm::vec3( 0.0f, 0.0f, -1.0f ),
		glm::vec3( 0.0f, 0.0f, -1.0f ),
		glm::vec3( 0.0f, 0.0f, -1.0f ),
		glm::vec3( 0.0f, 0.0f, -1.0f ),
		glm::vec3( 0.0f, 0.0f, -1.0f ),
		glm::vec3( 0.0f, 0.0f, -1.0f ),

		// left
		glm::vec3( -1.0f, 0.0f, 0.0f ),
		glm::vec3( -1.0f, 0.0f, 0.0f ),
		glm::vec3( -1.0f, 0.0f, 0.0f ),
		glm::vec3( -1.0f, 0.0f, 0.0f ),
		glm::vec3( -1.0f, 0.0f, 0.0f ),
		glm::vec3( -1.0f, 0.0f, 0.0f ),

		// right
		glm::vec3( 1.0f, 0.0f, 0.0f ),
		glm::vec3( 1.0f, 0.0f, 0.0f ),
		glm::vec3( 1.0f, 0.0f, 0.0f ),
		glm::vec3( 1.0f, 0.0f, 0.0f ),
		glm::vec3( 1.0f, 0.0f, 0.0f ),
		glm::vec3( 1.0f, 0.0f, 0.0f )
	};

	static const ConstBuffer<glm::vec3> s_unitCubeNormalsBuffer( s_unitCubeNormals, sizeof( s_unitCubeNormals ) / sizeof( glm::vec3 ) );

	static const glm::vec2 s_unitCubeUVs[] =
	{
		// U     V
		// bottom
		glm::vec2( 0.0f, 0.0f ),
		glm::vec2( 1.0f, 0.0f ),
		glm::vec2( 0.0f, 1.0f ),
		glm::vec2( 1.0f, 0.0f ),
		glm::vec2( 1.0f, 1.0f ),
		glm::vec2( 0.0f, 1.0f ),

		// top
		glm::vec2( 0.0f, 0.0f ),
		glm::vec2( 0.0f, 1.0f ),
		glm::vec2( 1.0f, 0.0f ),
		glm::vec2( 1.0f, 0.0f ),
		glm::vec2( 0.0f, 1.0f ),
		glm::vec2( 1.0f, 1.0f ),

		// front
		glm::vec2( 1.0f, 0.0f ),
		glm::vec2( 0.0f, 0.0f ),
		glm::vec2( 1.0f, 1.0f ),
		glm::vec2( 0.0f, 0.0f ),
		glm::vec2( 0.0f, 1.0f ),
		glm::vec2( 1.0f, 1.0f ),

		// back
		glm::vec2( 0.0f, 0.0f ),
		glm::vec2( 0.0f, 1.0f ),
		glm::vec2( 1.0f, 0.0f ),
		glm::vec2( 1.0f, 0.0f ),
		glm::vec2( 0.0f, 1.0f ),
		glm::vec2( 1.0f, 1.0f ),

		// left
		glm::vec2( 0.0f, 1.0f ),
		glm::vec2( 1.0f, 0.0f ),
		glm::vec2( 0.0f, 0.0f ),
		glm::vec2( 0.0f, 1.0f ),
		glm::vec2( 1.0f, 1.0f ),
		glm::vec2( 1.0f, 0.0f ),

		// right
		glm::vec2( 1.0f, 1.0f ),
		glm::vec2( 1.0f, 0.0f ),
		glm::vec2( 0.0f, 0.0f ),
		glm::vec2( 1.0f, 1.0f ),
		glm::vec2( 0.0f, 0.0f ),
		glm::vec2( 0.0f, 1.0f ),
	};

	Mesh* Mesh::Get( MeshHandle handle )
	{
		std::lock_guard<std::mutex> lock( m_instanceMutex );

		auto it = m_instances.find( handle.m_hash );
		if( it == m_instances.end() )
		{
			return nullptr;
		}

		return it->second;
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
			m_instances.insert( std::make_pair( hash, new Mesh( name, program ) ) );
		}

		MeshHandle handle;
		handle.m_hash = hash;

		return handle;
	}

	void Mesh::Destroy( MeshHandle handle )
	{
		Mesh* mesh = Get( handle );
		if( mesh != nullptr )
		{
			std::lock_guard<std::mutex> lock( m_instanceMutex );

			delete mesh;
			m_instances.erase( handle.m_hash );
		}
	}

	Mesh::Mesh( const char* name, ShaderHandle program ) :
		m_refCount( nullptr ),
		m_shader( program ),
		m_name( name )
	{
		DD_PROFILE_SCOPED( Mesh_Create );

		m_vao.Create();
		m_vboPosition.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );

		m_refCount = new std::atomic<int>( 1 );
	}

	Mesh::Mesh( const Mesh& other )
	{
		Assign( other );

		Retain();
	}

	Mesh::~Mesh()
	{
		Release();
	}

	Mesh& Mesh::operator=( const Mesh& other )
	{
		Release();

		Assign( other );

		Retain();

		return *this;
	}

	void Mesh::Assign( const Mesh& other )
	{
		m_vboPosition = other.m_vboPosition;
		m_bufferPosition = other.m_bufferPosition;

		m_useNormal = other.m_useNormal;
		m_vboNormal = other.m_vboNormal;
		m_bufferNormal = other.m_bufferNormal;

		m_useIndex = other.m_useIndex;
		m_vboIndex = other.m_vboIndex;
		m_bufferIndex = other.m_bufferIndex;

		m_useUV = other.m_useUV;
		m_vboUV = other.m_vboUV;
		m_bufferUV = other.m_bufferUV;

		m_useVertexColour = other.m_useVertexColour;
		m_vboVertexColour = other.m_vboVertexColour;
		m_bufferVertexColour = other.m_bufferVertexColour;

		m_vao = other.m_vao;

		m_name = other.m_name;
		m_shader = other.m_shader;
		m_bounds = other.m_bounds;

		m_colourMultiplier = other.m_colourMultiplier;

		m_refCount = other.m_refCount;
	}

	void Mesh::SetPositions( const ConstBuffer<glm::vec3>& positions )
	{
		m_bufferPosition = positions;

		m_vao.Bind();

		m_vboPosition.Bind();
		m_vboPosition.SetData( m_bufferPosition );

		m_shader.Get()->BindPositions();

		m_vao.Unbind();
	}

	void Mesh::SetNormals( const ConstBuffer<glm::vec3>& normals )
	{
		if( !m_vboNormal.IsValid() )
		{
			m_vboNormal.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_bufferNormal = normals;

		m_vao.Bind();

		m_vboNormal.Bind();
		m_vboNormal.SetData( m_bufferNormal );

		m_shader.Get()->BindNormals();

		m_vao.Unbind();
	}

	void Mesh::SetIndices( const ConstBuffer<uint>& indices )
	{
		if( !m_vboIndex.IsValid() )
		{
			m_vboIndex.Create( GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_bufferIndex = indices;

		m_vao.Bind();

		m_vboIndex.Bind();
		m_vboIndex.SetData( m_bufferIndex );

		m_vao.Unbind();
	}

	void Mesh::SetUVs( const ConstBuffer<glm::vec2>& uvs )
	{
		if( !m_vboUV.IsValid() )
		{
			m_vboUV.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_bufferUV = uvs;

		m_vao.Bind();

		m_vboUV.Bind();
		m_vboUV.SetData( m_bufferUV );

		m_shader.Get()->BindUVs();

		m_vao.Unbind();
	}

	void Mesh::SetVertexColours( const ConstBuffer<glm::vec4>& vertexColours )
	{
		if( !m_vboVertexColour.IsValid() )
		{
			m_vboVertexColour.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_bufferVertexColour = vertexColours;

		m_vao.Bind();

		m_vboVertexColour.Bind();
		m_vboVertexColour.SetData( m_bufferVertexColour );

		m_shader.Get()->BindVertexColours();

		m_vao.Unbind();
	}

	void Mesh::SetHeightColours( const glm::vec3* colours, const float* cutoffs, int count, const float& max_height )
	{
		m_bufferHeightColours.Set( colours, count );
		m_bufferHeightCutoffs.Set( cutoffs, count );
		m_maxHeight = &max_height;
	}

	void Mesh::UpdateBuffers()
	{
		m_vboPosition.Update();
		m_vboNormal.Update();
		m_vboIndex.Update();
		m_vboUV.Update();
		m_vboVertexColour.Update();
	}

	void Mesh::Render( const ICamera& camera, ShaderProgram& shader, const glm::mat4& transform )
	{
		DD_PROFILE_SCOPED( Mesh_Render );

		shader.SetUniform( "Model", transform );
		shader.SetUniform( "View", camera.GetCameraMatrix() );
		shader.SetUniform( "ViewPosition", camera.GetPosition() );
		shader.SetUniform( "Projection", camera.GetProjectionMatrix() );
		shader.SetUniform( "NormalMatrix", glm::transpose( glm::inverse( glm::mat3( transform ) ) ) );
		shader.SetUniform( "ObjectColour", m_colourMultiplier );

		if( m_useHeightColours )
		{
			for( int i = 0; i < m_bufferHeightColours.Size(); ++i )
			{
				shader.SetUniform( GetArrayUniformName( "HeightLevels", i, "Colour" ).c_str(), m_bufferHeightColours[ i ] );
				shader.SetUniform( GetArrayUniformName( "HeightLevels", i, "Cutoff" ).c_str(), m_bufferHeightCutoffs[ i ] );
			}

			shader.SetUniform( "HeightCount", m_bufferHeightColours.Size() );
			shader.SetUniform( "MaxHeight", *m_maxHeight );
		}

		m_vao.Bind();

		if( !m_useIndex )
		{
			glDrawArrays( GL_TRIANGLES, 0, m_bufferPosition.Size() );
			CheckGLError();
		}
		else
		{
			glDrawElements( GL_TRIANGLES, m_bufferIndex.Size(), GL_UNSIGNED_INT, 0 );
			CheckGLError();
		}

		m_vao.Unbind();
	}

	void Mesh::Retain()
	{ 
		DD_ASSERT( m_refCount != nullptr );

		++*m_refCount;
	}

	void Mesh::Release()
	{
		DD_ASSERT( m_refCount != nullptr );

		if( --*m_refCount <= 0 )
		{
			m_vboPosition.Destroy();
			m_vboIndex.Destroy();
			m_vboNormal.Destroy();
			m_vboUV.Destroy();
			m_vboVertexColour.Destroy();

			m_vao.Destroy();

			delete m_refCount;
		}
	}

	void Mesh::UseShader( bool use )
	{
		m_shader.Get()->Use( use );
	}

	void Mesh::MakeUnitCube()
	{
		ShaderProgram& shader = *m_shader.Get();
		shader.Use( true );

		shader.SetPositionsName( "Position" );
		SetPositions( s_unitCubePositionsBuffer );

		shader.SetNormalsName( "Normal" );

		SetNormals( s_unitCubeNormalsBuffer );

		//shader.SetUVsName( "UV" );

		//Buffer<glm::vec2> uvs( const_cast<glm::vec2*>(s_unitCubeUVs), sizeof( s_unitCubeUVs ) / sizeof( glm::vec2 ) );
		//SetUVs( uvs );

		shader.Use( false );

		AABB bounds;
		bounds.Expand( glm::vec3( -1, -1, -1 ) );
		bounds.Expand( glm::vec3( 1, 1, 1 ) );
		SetBounds( bounds );
	}
}