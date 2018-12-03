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
		if( !m_vboPosition.IsValid() )
		{
			m_vboPosition.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vboPosition.SetData( positions );

		m_dirty = true;
	}

	void Mesh::SetNormals( const dd::ConstBuffer<glm::vec3>& normals )
	{
		if( !m_vboNormal.IsValid() )
		{
			m_vboNormal.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vboNormal.SetData( normals );

		m_dirty = true;
	}

	void Mesh::SetIndices( const dd::ConstBuffer<uint>& indices )
	{
		if( !m_vboIndex.IsValid() )
		{
			m_vboIndex.Create( GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vboIndex.SetData( indices );

		m_dirty = true;
	}

	void Mesh::SetUVs( const dd::ConstBuffer<glm::vec2>& uvs )
	{
		if( !m_vboUV.IsValid() )
		{
			m_vboUV.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vboUV.SetData( uvs );

		m_dirty = true;
	}

	void Mesh::SetVertexColours( const dd::ConstBuffer<glm::vec4>& colours )
	{
		if( !m_vboVertexColour.IsValid() )
		{
			m_vboVertexColour.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		}

		m_vboVertexColour.SetData( colours );

		m_dirty = true;
	}

	void Mesh::SetMaterial( MaterialHandle material )
	{
		m_material = material;
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
		if( !m_vao.IsValid() )
		{
			return;
		}

		if( m_dirty )
		{
			if( m_vboPosition.IsValid() )
			{
				m_vboPosition.Bind();
				m_vboPosition.CommitData();
				m_vboPosition.Unbind();
			}

			if( m_vboNormal.IsValid() )
			{
				m_vboNormal.Bind();
				m_vboNormal.CommitData();
				m_vboNormal.Unbind();
			}

			if( m_vboIndex.IsValid() )
			{
				m_vboIndex.Bind();
				m_vboIndex.CommitData();
				m_vboIndex.Unbind();
			}

			if( m_vboUV.IsValid() )
			{
				m_vboUV.Bind();
				m_vboUV.CommitData();
				m_vboUV.Unbind();
			}

			if( m_vboVertexColour.IsValid() )
			{
				m_vboVertexColour.Bind();
				m_vboVertexColour.CommitData();
				m_vboVertexColour.Unbind();
			}

			if( !m_rebuilding.exchange( true ) )
			{
				if( m_bvh != nullptr )
				{
					delete m_bvh;
				}

				jobsystem.Schedule( [this]() { RebuildBVH(); } );
			}

			m_dirty = false;
		}
	}

	void Mesh::BindToShader( Shader& shader )
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

	void Mesh::Render( UniformStorage& uniforms, Shader& shader, const glm::mat4& transform )
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