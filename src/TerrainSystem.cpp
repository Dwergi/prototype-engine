//
// TerrainSystem.cpp - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PrecompiledHeader.h"
#include "TerrainSystem.h"

#include "Camera.h"
#include "JobSystem.h"
#include "TerrainChunk.h"

#include "Stream.h"

namespace dd
{
	TerrainSystem::TerrainSystem( Camera& camera ) :
		m_camera( camera ),
		m_chunkSize( LowDetailChunkSize )
	{
		DD_ASSERT( LowDetailChunkSize / (1 << (LODLevels - 1)) >= 2 );
	}

	TerrainSystem::~TerrainSystem()
	{
		for( auto& chunk : m_activeChunks )
		{
			delete chunk.Value;
		}

		for( auto& chunk : m_inactiveChunks )
		{
			delete chunk.Value;
		}

		m_activeChunks.Clear();
	}

	float DistanceTo( const glm::vec3& pos, const TerrainChunkKey& key )
	{
		glm::vec3 chunk_pos( key.X + key.Size / 2, 0, key.Y + key.Size / 2 );

		return glm::distance( pos, chunk_pos );
	}

	void SplitChunk( TerrainChunkKey& chunk, Vector<TerrainChunkKey>& split_into )
	{
		chunk.IsSplit = true;

		uint new_size = chunk.Size / 2;

		TerrainChunkKey& bottom_left = split_into.Allocate();
		bottom_left.Size = new_size;
		bottom_left.X = chunk.X;
		bottom_left.Y = chunk.Y;

		TerrainChunkKey& bottom_right = split_into.Allocate();
		bottom_right.Size = new_size;
		bottom_right.X = chunk.X + new_size;
		bottom_right.Y = chunk.Y;

		TerrainChunkKey& top_left = split_into.Allocate();
		top_left.Size = new_size;
		top_left.X = chunk.X;
		top_left.Y = chunk.Y + new_size;

		TerrainChunkKey& top_right = split_into.Allocate();
		top_right.Size = new_size;
		top_right.X = chunk.X + new_size;
		top_right.Y = chunk.Y + new_size;
	}

	void CreateLowDetailChunks( Vector<TerrainChunkKey>& chunks, glm::vec2 chunk_origin, uint chunk_size, uint chunks_per_dim )
	{
		int half_chunks = (int) chunks_per_dim / 2;
		int low_detail_chunk_size = (int) chunk_size;

		for( int y = -half_chunks; y < half_chunks; ++y )
		{
			for( int x = -half_chunks; x < half_chunks; ++x )
			{
				TerrainChunkKey& key = chunks.Allocate();
				key.Size = low_detail_chunk_size;
				key.X = int( chunk_origin.x ) + x * low_detail_chunk_size;
				key.Y = int( chunk_origin.y ) + y * low_detail_chunk_size;
			}
		}
	}

	struct ChunkDistance
	{
		TerrainChunkKey* Chunk;
		float Distance;
	};

	//
	// Find the closest num_to_split chunks in chunks to the cam_pos, and create new chunks into split_into.
	//
	void SplitChunks( Vector<TerrainChunkKey>& chunks, Vector<TerrainChunkKey>& split_into, const glm::vec3& cam_pos, uint num_to_split )
	{
		Vector<ChunkDistance> distances;
		distances.Reserve( chunks.Size() );

		for( TerrainChunkKey& chunk : chunks )
		{
			ChunkDistance& dist = distances.Allocate();
			dist.Chunk = &chunk;
			dist.Distance = DistanceTo( cam_pos, chunk );
		}

		for( uint i = 0; i < num_to_split; ++i )
		{
			ChunkDistance closest;
			closest.Chunk = nullptr;
			closest.Distance = std::numeric_limits<float>::max();
			for( const ChunkDistance& chunk_dist : distances )
			{
				if( chunk_dist.Distance < closest.Distance && !chunk_dist.Chunk->IsSplit )
				{
					closest.Distance = chunk_dist.Distance;
					closest.Chunk = chunk_dist.Chunk;
				}
			}

			if( closest.Chunk != nullptr )
			{
				SplitChunk( *closest.Chunk, split_into );
			}
		}
	}

	//
	// Find or create a single chunk of terrain.
	// If an inactive chunk is found, it is moved to m_activeChunks.
	// If an active chunk is found, it stays in m_activeChunks.
	//
	void TerrainSystem::GenerateChunk( const TerrainChunkKey& chunk, DenseMap<TerrainChunkKey, TerrainChunk*>& activeChunks )
	{
		// find existing chunk
		TerrainChunk** terrain_chunk = activeChunks.Find( chunk );
		if( terrain_chunk != nullptr )
		{
			// found, add it to the active chunks and continue
			activeChunks.Remove( chunk );
			m_activeChunks.Add( chunk, *terrain_chunk );
		}
		else
		{
			// doesn't exist, find inactive chunk
			terrain_chunk = m_inactiveChunks.Find( chunk );
			if( terrain_chunk != nullptr )
			{
				// found, add it to the active chunks and continue
				m_inactiveChunks.Remove( chunk );
				m_activeChunks.Add( chunk, *terrain_chunk );
			}
			else
			{
				// doesn't exist, create new
				TerrainChunk* new_chunk = new TerrainChunk( chunk );
				m_activeChunks.Add( chunk, new_chunk );

				JobSystem& jobsystem = Services::Get<JobSystem>();
				jobsystem.Schedule( std::bind( &TerrainChunk::Generate, new_chunk ), "TerrainGeneration" );
			}
		}
	}

	void TerrainSystem::GenerateTerrain( const Vector<Vector<TerrainChunkKey>>& chunks, DenseMap<TerrainChunkKey, TerrainChunk*>& activeChunks )
	{
		for( const Vector<TerrainChunkKey>& lod_level : chunks )
		{
			for( const TerrainChunkKey& chunk : lod_level )
			{
				if( !chunk.IsSplit )
				{
					GenerateChunk( chunk, activeChunks );
				}
			}
		}
	}

	void TerrainSystem::PurgeInactiveChunks()
	{
		DenseMap<TerrainChunkKey, float> distances;

		for( auto& entry : m_inactiveChunks )
		{
			distances.Add( entry.Key, DistanceTo( m_camera.GetPosition(), entry.Key ) );
		}

		// TODO: This is probably slow as balls. 
		while( m_inactiveChunks.Size() > MaxInactiveChunks )
		{
			TerrainChunkKey closest;
			float closest_distance = std::numeric_limits<float>::max();

			for( auto& entry : distances )
			{
				if( entry.Value < closest_distance )
				{
					m_inactiveChunks.Remove( entry.Key );
					distances.Remove( entry.Key );
					break;
				}
			}
		}
	}

	void TerrainSystem::Initialize()
	{
		Update( 0 );
	}

	void TerrainSystem::Update( float delta_t )
	{
		glm::vec3 cam_pos = m_camera.GetPosition();
		glm::vec2 chunk_origin = glm::vec2( (int) (cam_pos.x / m_chunkSize) * (int) m_chunkSize, (int) (cam_pos.z / m_chunkSize) * (int) m_chunkSize );

		// generate chunk keys
		Vector<Vector<TerrainChunkKey>> chunks;
		chunks.Resize( LODLevels );

		// create just low detail chunks
		CreateLowDetailChunks( chunks[LODLevels - 1], chunk_origin, m_chunkSize, LowDetailChunksPerDim );

		// split it into higher detail
		for( int lod = LODLevels - 1; lod > 0; --lod )
		{
			SplitChunks( chunks[lod], chunks[lod - 1], cam_pos, ChunksToSplit );
		}

		DenseMap<TerrainChunkKey, TerrainChunk*> activeChunks( std::move( m_activeChunks ) );
		m_activeChunks.Clear();

		GenerateTerrain( chunks, activeChunks );

		// Move previously active chunks to inactive
		for( auto& entry : activeChunks )
		{
			m_inactiveChunks.Add( entry.Key, entry.Value );
		}

		PurgeInactiveChunks();
	}

	void TerrainSystem::WaitForGeneration() const
	{
		JobSystem& jobsystem = Services::Get<JobSystem>();
		jobsystem.WaitForCategory( "TerrainGeneration" );
	}

	void TerrainSystem::Render( Camera& camera, ShaderProgram& shader )
	{
		for( auto& chunk : m_activeChunks )
		{
			chunk.Value->CreateRenderResources( shader );
			chunk.Value->Render( camera );
		}
	}

	void TerrainSystem::SaveChunkImages() const
	{
		WaitForGeneration();

		String64 filename( "chunk_" );

		int chunk_index = 0;

		for( auto& chunk : m_activeChunks )
		{
			String64 chunk_file( filename );
			WriteStream write( chunk_file );
			write.WriteFormat( "%d.tga", chunk_index );

			chunk.Value->Write( chunk_file.c_str() );

			++chunk_index;
		}
	}

	void TerrainSystem::SetChunkSize( uint size )
	{
		m_chunkSize = size;
	}
}
