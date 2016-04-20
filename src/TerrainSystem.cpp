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
		for( TerrainChunk* chunk : m_chunks )
		{
			delete chunk;
		}

		m_chunks.Clear();
	}

	float DistanceTo( const glm::vec3& pos, const ChunkKey& key )
	{
		glm::vec3 chunk_pos( key.X + key.Size / 2, 0, key.Y + key.Size / 2 );

		return glm::distance( pos, chunk_pos );
	}

	void SplitChunk( ChunkKey& chunk, Vector<ChunkKey>& split_into )
	{
		chunk.IsSplit = true;

		uint new_size = chunk.Size / 2;

		ChunkKey& bottom_left = split_into.Allocate();
		bottom_left.Size = new_size;
		bottom_left.X = chunk.X;
		bottom_left.Y = chunk.Y;

		ChunkKey& bottom_right = split_into.Allocate();
		bottom_right.Size = new_size;
		bottom_right.X = chunk.X + new_size;
		bottom_right.Y = chunk.Y;

		ChunkKey& top_left = split_into.Allocate();
		top_left.Size = new_size;
		top_left.X = chunk.X;
		top_left.Y = chunk.Y + new_size;

		ChunkKey& top_right = split_into.Allocate();
		top_right.Size = new_size;
		top_right.X = chunk.X + new_size;
		top_right.Y = chunk.Y + new_size;
	}

	void CreateLowDetailChunks( Vector<ChunkKey>& chunks, glm::vec2 chunk_origin, uint chunk_size, uint chunks_per_dim )
	{
		int half_chunks = (int) chunks_per_dim / 2;
		int low_detail_chunk_size = (int) chunk_size;

		for( int y = -half_chunks; y < half_chunks; ++y )
		{
			for( int x = -half_chunks; x < half_chunks; ++x )
			{
				ChunkKey& key = chunks.Allocate();
				key.Size = low_detail_chunk_size;
				key.X = int( chunk_origin.x ) + x * low_detail_chunk_size;
				key.Y = int( chunk_origin.y ) + y * low_detail_chunk_size;
			}
		}
	}

	struct ChunkDistance
	{
		ChunkKey* Chunk;
		float Distance;
	};

	//
	// Find the closest num_to_split chunks in chunks to the cam_pos, and create new chunks into split_into.
	//
	void SplitChunks( Vector<ChunkKey>& chunks, Vector<ChunkKey>& split_into, const glm::vec3& cam_pos, uint num_to_split )
	{
		Vector<ChunkDistance> distances;
		distances.Reserve( chunks.Size() );

		for( ChunkKey& chunk : chunks )
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

			SplitChunk( *closest.Chunk, split_into );
		}
	}

	void TerrainSystem::Initialize()
	{
		glm::vec3 cam_pos = m_camera.GetPosition();
		glm::vec2 chunk_origin = glm::vec2( (uint) (cam_pos.x / m_chunkSize) * m_chunkSize, (uint) (cam_pos.z / m_chunkSize) * m_chunkSize );

		// generate low-detail chunks
		Vector<Vector<ChunkKey>> chunks;
		chunks.Resize( LODLevels );

		CreateLowDetailChunks( chunks[LODLevels - 1], chunk_origin, m_chunkSize, LowDetailChunksPerDim );

		for( int lod = LODLevels - 1; lod > 0; --lod )
		{
			SplitChunks( chunks[lod], chunks[lod-1], cam_pos, ChunksToSplit );
		}

		for( Vector<ChunkKey>& lod_level : chunks )
		{
			for( const ChunkKey& chunk : lod_level )
			{
				if( !chunk.IsSplit )
				{
					TerrainChunk* terrain_chunk = new TerrainChunk( chunk );
					m_chunks.Add( terrain_chunk );

					JobSystem& jobsystem = Services::Get<JobSystem>();
					jobsystem.Schedule( std::bind( &TerrainChunk::Generate, terrain_chunk ), "TerrainGeneration" );
				}
			}
		}
	}

	void TerrainSystem::Update( float delta_t )
	{
		
	}

	void TerrainSystem::Render( Camera& camera, ShaderProgram& shader )
	{
		for( TerrainChunk* chunk : m_chunks )
		{
			chunk->CreateRenderResources( shader );
			chunk->Render( camera );
		}
	}

	void TerrainSystem::SaveChunkImages() const
	{
		JobSystem& jobsystem = Services::Get<JobSystem>();
		jobsystem.WaitForCategory( "TerrainGeneration" );

		String64 filename( "chunk_" );

		int chunk_index = 0;

		for( TerrainChunk* chunk : m_chunks )
		{
			String64 chunk_file( filename );
			WriteStream write( chunk_file );
			write.WriteFormat( "%d.tga", chunk_index );

			chunk->Write( chunk_file.c_str() );

			++chunk_index;
		}
	}

	void TerrainSystem::SetChunkSize( uint size )
	{
		m_chunkSize = size;
	}
}
