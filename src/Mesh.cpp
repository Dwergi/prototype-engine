//
// Mesh.cpp - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#include "PrecompiledHeader.h"
#include "Mesh.h"

#include "ICamera.h"
#include "GLError.h"
#include "Material.h"
#include "MeshHandle.h"
#include "ShaderProgram.h"
#include "Uniforms.h"
#include "WorldRenderer.h"

#include "GL/gl3w.h"

#include "glm/gtx/transform.hpp"

namespace ddr
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

	static dd::ConstBuffer<glm::vec3> s_unitCubePositionsBuffer( s_unitCubePositions, sizeof( s_unitCubePositions ) / sizeof( glm::vec3 ) );

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

	static const dd::ConstBuffer<glm::vec3> s_unitCubeNormalsBuffer( s_unitCubeNormals, sizeof( s_unitCubeNormals ) / sizeof( glm::vec3 ) );

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

	static const dd::ConstBuffer<glm::vec2> s_unitCubeUVsBuffer( s_unitCubeUVs, sizeof( s_unitCubeUVs ) / sizeof( glm::vec2 ) );

	Mesh* Mesh::Get( MeshHandle handle )
	{
		std::lock_guard lock( m_instanceMutex );

		auto it = m_instances.find( handle.m_hash );
		if( it == m_instances.end() )
		{
			return nullptr;
		}

		return it->second;
	}

	MeshHandle Mesh::Create( const char* name )
	{
		MeshHandle mesh_h = Find( name );
		if( !mesh_h.IsValid() )
		{
			std::lock_guard lock( m_instanceMutex );

			mesh_h.m_hash = dd::HashString( name, strlen( name ) );

			m_instances.insert( std::make_pair( mesh_h.m_hash, new Mesh( name ) ) );
		}

		return mesh_h;
	}

	MeshHandle Mesh::Find( const char* name )
	{
		DD_ASSERT( name != nullptr );
		DD_ASSERT( strlen( name ) > 0 );

		uint64 hash = dd::HashString( name, strlen( name ) );

		std::lock_guard lock( m_instanceMutex );

		MeshHandle mesh_h;

		auto it = m_instances.find( hash );
		if( it != m_instances.end() )
		{
			mesh_h.m_hash = hash;
		}

		return mesh_h;
	}

	void Mesh::Destroy( MeshHandle mesh_h )
	{
		Mesh* mesh = Get( mesh_h );
		if( mesh != nullptr )
		{
			std::lock_guard lock( m_instanceMutex );

			delete mesh;
			m_instances.erase( mesh_h.m_hash );
		}
	}

	Mesh::Mesh( const char* name ) :
		m_name( name )
	{
		DD_PROFILE_SCOPED( Mesh_Create );

		m_vao.Create();
	}

	Mesh::~Mesh()
	{
		m_vboPosition.Destroy();
		m_vboIndex.Destroy();
		m_vboNormal.Destroy();
		m_vboUV.Destroy();
		m_vboVertexColour.Destroy();

		m_vao.Destroy();
	}

	void Mesh::SetPositions( const dd::ConstBuffer<glm::vec3>& positions )
	{
		if( !m_vboPosition.IsValid() )
		{
			m_vboPosition.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vao.Bind();

		m_vboPosition.Bind();
		m_vboPosition.SetData( positions );
		m_vboPosition.Unbind();
		
		m_vao.Unbind();
	}

	void Mesh::SetNormals( const dd::ConstBuffer<glm::vec3>& normals )
	{
		if( !m_vboNormal.IsValid() )
		{
			m_vboNormal.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vao.Bind();

		m_vboNormal.Bind();
		m_vboNormal.SetData( normals );
		m_vboNormal.Unbind();
		
		m_vao.Unbind();
	}

	void Mesh::SetIndices( const dd::ConstBuffer<uint>& indices )
	{
		if( !m_vboIndex.IsValid() )
		{
			m_vboIndex.Create( GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vao.Bind();

		m_vboIndex.Bind();
		m_vboIndex.SetData( indices );
		m_vboIndex.Unbind();

		m_vao.Unbind();
	}

	void Mesh::SetUVs( const dd::ConstBuffer<glm::vec2>& uvs )
	{
		if( !m_vboUV.IsValid() )
		{
			m_vboUV.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vao.Bind();

		m_vboUV.Bind();
		m_vboUV.SetData( uvs );
		m_vboUV.Unbind();

		m_vao.Unbind();
	}

	void Mesh::SetVertexColours( const dd::ConstBuffer<glm::vec4>& colours )
	{
		if( !m_vboVertexColour.IsValid() )
		{
			m_vboVertexColour.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vao.Bind();

		m_vboVertexColour.Bind();
		m_vboVertexColour.SetData( colours );

		m_vao.Unbind();
	}

	void Mesh::SetMaterial( MaterialHandle material )
	{
		m_material = material;
	}

	void Mesh::UpdateBuffers()
	{
		m_vboPosition.UpdateData();
		m_vboNormal.UpdateData();
		m_vboIndex.UpdateData();
		m_vboUV.UpdateData();
		m_vboVertexColour.UpdateData();

		m_dirty = false;
	}

	void Mesh::BindToShader( ShaderProgram& shader )
	{
		if( m_vboPosition.IsValid() )
		{
			m_vboPosition.Bind();
			shader.BindPositions();
			m_vboPosition.Unbind();
		}

		if( m_vboNormal.IsValid() )
		{
			m_vboNormal.Bind();
			shader.BindNormals();
			m_vboNormal.Unbind();
		}

		if( m_vboUV.IsValid() )
		{
			m_vboUV.Bind();
			shader.BindUVs();
			m_vboUV.Unbind();
		}

		if( m_vboVertexColour.IsValid() )
		{
			m_vboVertexColour.Bind();
			shader.BindVertexColours();
			m_vboVertexColour.Unbind();
		}
	}

	void Mesh::Render( UniformStorage& uniforms, ShaderProgram& shader, const glm::mat4& transform )
	{
		DD_PROFILE_SCOPED( Mesh_Render );

		DD_ASSERT( shader.InUse() );

		m_vao.Bind();

		uniforms.Set( "Model", transform );
		uniforms.Set( "NormalMatrix", glm::transpose( glm::inverse( glm::mat3( transform ) ) ) );

		uniforms.Bind( shader );

		BindToShader( shader );

		if( m_vboIndex.IsValid() )
		{
			m_vboIndex.Bind();

			glDrawElements( GL_TRIANGLES, m_vboIndex.GetDataSize(), GL_UNSIGNED_INT, 0 );
			CheckOGLError();

			m_vboIndex.Unbind();
		}
		else
		{
			glDrawArrays( GL_TRIANGLES, 0, m_vboPosition.GetDataSize() );
			CheckOGLError();
		}

		m_vao.Unbind();
	}

	void Mesh::MakeUnitCube()
	{
		SetPositions( s_unitCubePositionsBuffer );
		SetNormals( s_unitCubeNormalsBuffer );
		//SetUVs( s_unitCubeUVsBuffer );

		dd::AABB bounds;
		bounds.Expand( glm::vec3( -1, -1, -1 ) );
		bounds.Expand( glm::vec3( 1, 1, 1 ) );
		SetBounds( bounds );
	}
}