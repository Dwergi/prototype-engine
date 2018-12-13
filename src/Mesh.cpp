//
// Mesh.cpp - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#include "PCH.h"
#include "Mesh.h"

#include "BVHTree.h"
#include "ICamera.h"
#include "JobSystem.h"
#include "Material.h"
#include "OpenGL.h"
#include "Shader.h"
#include "Triangulator.h"
#include "Uniforms.h"
#include "WorldRenderer.h"

DD_HANDLE_MANAGER( ddr::Mesh );

namespace ddr
{
	Mesh::Mesh()
	{
		DD_PROFILE_SCOPED( Mesh_Create );
	}

	Mesh::~Mesh()
	{
		DD_ASSERT_ERROR( !m_vao.IsValid() );
	}

	void Mesh::SetPositions( const dd::ConstBuffer<glm::vec3>& positions )
	{
		DD_ASSERT( m_vao.IsValid() );

		if( !m_vboPosition.IsValid() )
		{
			m_vboPosition.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vboPosition.SetData( positions );

		m_dirty.set( (size_t) BufferType::Position, true );
	}

	void Mesh::SetNormals( const dd::ConstBuffer<glm::vec3>& normals )
	{
		DD_ASSERT( m_vao.IsValid() );

		if( !m_vboNormal.IsValid() )
		{
			m_vboNormal.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vboNormal.SetData( normals );

		m_dirty.set( (size_t) BufferType::Normal, true );
	}

	void Mesh::SetIndices( const dd::ConstBuffer<uint>& indices )
	{
		DD_ASSERT( m_vao.IsValid() );

		m_vao.Bind();

		if( !m_vboIndex.IsValid() )
		{
			m_vboIndex.Create( GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW );
			m_vboIndex.Bind();
		}

		m_vboIndex.SetData( indices );

		m_dirty.set( (size_t) BufferType::Index, true );

		m_vao.Unbind();
		m_vboIndex.Unbind();
	}

	void Mesh::SetUVs( const dd::ConstBuffer<glm::vec2>& uvs )
	{
		DD_ASSERT( m_vao.IsValid() );

		if( !m_vboUV.IsValid() )
		{
			m_vboUV.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vboUV.SetData( uvs );

		m_dirty.set( (size_t) BufferType::UV, true );
	}

	void Mesh::SetVertexColours( const dd::ConstBuffer<glm::vec4>& colours )
	{
		DD_ASSERT( m_vao.IsValid() );

		if( !m_vboVertexColour.IsValid() )
		{
			m_vboVertexColour.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vboVertexColour.SetData( colours );

		m_dirty.set( (size_t) BufferType::Colour, true );
	}

	void Mesh::SetMaterial( MaterialHandle material )
	{
		m_material = material;

		Shader* shader = material.Get()->Shader.Access();
		DD_ASSERT( shader != nullptr );

		BindToShader( *shader );
	}

	void Mesh::Create()
	{
		DD_ASSERT( !m_vao.IsValid() );

		m_vao.Create();
	}

	void Mesh::Destroy()
	{
		m_vboPosition.Destroy();
		m_vboIndex.Destroy();
		m_vboNormal.Destroy();
		m_vboUV.Destroy();
		m_vboVertexColour.Destroy();

		m_vao.Destroy();
	}

	void Mesh::Update( dd::JobSystem& jobsystem )
	{
		DD_ASSERT( m_vao.IsValid() );

		m_vao.Bind();

		if( m_dirty.test( (size_t) BufferType::Position ) && 
			m_vboPosition.IsValid() )
		{
			m_vboPosition.CommitData();
		}

		if( m_dirty.test( (size_t) BufferType::Normal ) && 
			m_vboNormal.IsValid() )
		{
			m_vboNormal.CommitData();
		}

		if( m_dirty.test( (size_t) BufferType::Index ) && 
			m_vboIndex.IsValid() )
		{
			m_vboIndex.CommitData();
		}

		if( m_dirty.test( (size_t) BufferType::UV ) && 
			m_vboUV.IsValid() )
		{
			m_vboUV.CommitData();
		}

		if( m_dirty.test( (size_t) BufferType::Colour ) && 
			m_vboVertexColour.IsValid() )
		{
			m_vboVertexColour.CommitData();
		}

		m_vao.Unbind();

		if( m_enableBVH && !m_rebuilding.exchange( true ) )
		{
			if( m_bvh != nullptr )
			{
				delete m_bvh;
			}

			jobsystem.Schedule( [this]() { RebuildBVH(); } );
		}

		m_dirty.reset();
	}

	void Mesh::BindToShader( Shader& shader )
	{
		DD_ASSERT( shader.InUse() );
		DD_ASSERT( m_vao.IsValid() );

		m_vao.Bind();

		if( m_vboPosition.IsValid() )
		{
			shader.BindPositions();
		}

		if( m_vboNormal.IsValid() )
		{
			shader.BindNormals();
		}

		if( m_vboUV.IsValid() )
		{
			shader.BindUVs();
		}

		if( m_vboVertexColour.IsValid() )
		{
			shader.BindVertexColours();
		}

		m_vao.Unbind();
	}

	void Mesh::Render()
	{
		DD_PROFILE_SCOPED( Mesh_Render );

		m_vao.Bind();

		if( m_vboIndex.IsValid() )
		{
			glDrawElements( GL_TRIANGLES, m_vboIndex.GetDataSize(), GL_UNSIGNED_INT, 0 );
			CheckOGLError();
		}
		else
		{
			glDrawArrays( GL_TRIANGLES, 0, m_vboPosition.GetDataSize() );
			CheckOGLError();
		}

		m_vao.Unbind();
	}

	void Mesh::RebuildBVH()
	{
		DD_ASSERT( m_rebuilding );

		m_bvh = new dd::BVHTree();
		m_bvh->StartBatch();

		const dd::ConstBuffer<glm::vec3>& positions = GetPositions();
		const dd::ConstBuffer<uint>& indices = GetIndices();

		dd::ConstTriangulator triangulator( positions, indices );
		for( size_t i = 0; i < triangulator.Size(); ++i )
		{
			dd::ConstTriangle tri = triangulator[ i ];

			ddm::AABB bounds;
			bounds.Expand( tri.p0 );
			bounds.Expand( tri.p1 );
			bounds.Expand( tri.p2 );

			m_bvh->Add( bounds );
		}

		m_bvh->EndBatch();

		m_rebuilding = false;
	}

	void MeshManager::OnCreate( Mesh& mesh ) const
	{
		mesh.Create();
	}

	void MeshManager::OnDestroy( Mesh& mesh ) const
	{
		mesh.Destroy();
	}
}