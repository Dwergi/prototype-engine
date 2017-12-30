//
// TerrainSystem.cpp - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PrecompiledHeader.h"
#include "TerrainSystem.h"

#include "Camera.h"
#include "JobSystem.h"
#include "Stream.h"
#include "TerrainChunk.h"

#include <algorithm>

#include "imgui/imgui.h"

namespace dd
{
	const float TerrainSystem::DefaultVertexDistance = 0.5f;

	TerrainSystem::TerrainSystem( Camera& camera, JobSystem& jobSystem ) :
		m_camera( camera ),
		m_jobSystem( jobSystem )
	{
		SetVertexDistance( DefaultVertexDistance );
		SetLODLevels( DefaultLODLevels );
	}

	TerrainSystem::~TerrainSystem()
	{
		
	}

	static float DistanceTo( const glm::vec3& pos, const TerrainChunkKey& key )
	{
		glm::vec3 chunk_pos( key.X + key.Size / 2, 0, key.Y + key.Size / 2 );

		return glm::distance( pos, chunk_pos );
	}

	void TerrainSystem::ClearChunks( EntityManager& entityManager )
	{
		for( auto& chunk : m_chunks )
		{
			chunk.second->Destroy( entityManager );
			delete chunk.second;
		}

		m_chunks.clear();
	}

	void TerrainSystem::SetVertexDistance( float distance )
	{
		m_vertexDistance = distance;
		m_requiresRegeneration = true;
	}

	void TerrainSystem::SetLODLevels( int lods )
	{
		m_lodLevels = lods;
		m_requiresRegeneration = true;
	}

	void TerrainSystem::Initialize( EntityManager& entityManager )
	{
		TerrainChunk::GenerateSharedResources();

		Update( entityManager, 0 );
	}

	void TerrainSystem::Update( EntityManager& entityManager, float delta_t )
	{
		if( m_requiresRegeneration )
		{
			ClearChunks( entityManager );

			GenerateTerrain( entityManager );

			m_requiresRegeneration = false;
		}
	}

	void TerrainSystem::SaveChunkImages() const
	{
		String64 filename( "chunk_" );

		int chunk_index = 0;

		for( auto& chunk : m_chunks )
		{
			String64 chunk_file( filename );
			WriteStream write( chunk_file );
			write.WriteFormat( "terrain_%d.tga", chunk_index );

			chunk.second->Write( chunk_file.c_str() );

			++chunk_index;
		}
	}
	
	void TerrainSystem::GenerateTerrain( EntityManager& entityManager )
	{
		// start with a 4x4 grid of the lowest LOD level, 
		// then at each level split the centermost 2x2 grid into a 4x4 grid of one LOD level lower
		
		for( int lod = m_lodLevels - 1; lod >= 0; --lod )
		{
			GenerateLODLevel( entityManager, lod );
		}
	}

	void TerrainSystem::GenerateLODLevel( EntityManager& entityManager, int lod )
	{
		const float vertexDistance = m_vertexDistance * (1 << lod);
		const float chunkSize = TerrainChunk::Vertices * vertexDistance;

		DD_PROFILE_START( TerrainSystem_GenerateLODLevel );

		for( int y = -ChunksPerDimension / 2; y < ChunksPerDimension / 2; ++y )
		{
			for( int x = -ChunksPerDimension / 2; x < ChunksPerDimension / 2; ++x )
			{
				if( lod != 0 )
				{
					// don't generate chunks for the middle-most grid unless we're at LOD 0
					if( (x == -1 || x == 0) &&
						(y == -1 || y == 0) )
					{
						continue;
					}
				}

				TerrainChunkKey key;
				key.X = x * chunkSize;
				key.Y = y * chunkSize;
				key.LOD = lod;
				key.Size = vertexDistance;

				TerrainChunk* chunk = GenerateChunk( entityManager, key );

				m_chunks.insert( std::make_pair( key, chunk ) );
			}
		}

		DD_PROFILE_END();
	}

	TerrainChunk* TerrainSystem::GenerateChunk( EntityManager& entityManager, const TerrainChunkKey& key )
	{
		TerrainChunk* chunk = new TerrainChunk( key );
		chunk->Generate( entityManager );

		return chunk;
	}

	void TerrainSystem::DrawDebugInternal()
	{
		if( ImGui::DragInt( "LODs", &m_lodLevels, 1, 1, 10 ) )
		{
			SetLODLevels( m_lodLevels );
		}

		if( ImGui::DragFloat( "Vertex Distance", &m_vertexDistance, 0.05f, 0.01f, 2.0f ) )
		{
			SetVertexDistance( m_vertexDistance );
		}
	}
}
