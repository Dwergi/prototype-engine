//
// TerrainSystem.cpp - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PrecompiledHeader.h"
#include "TerrainSystem.h"

#include "BoundsComponent.h"
#include "ICamera.h"
#include "JobSystem.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "OpenGL.h"
#include "Random.h"
#include "RenderData.h"
#include "Stream.h"
#include "TerrainChunk.h"
#include "TerrainChunkComponent.h"
#include "TransformComponent.h"
#include "Uniforms.h"

#include <algorithm>

#include "imgui/imgui.h"

#include "glm/gtc/type_ptr.hpp"

namespace dd
{
	static glm::vec4 GetMeshColour( const TerrainChunkKey& key )
	{
		glm::vec4 colour( 0, 0, 0, 1 );

		int element = key.LOD % 3;
		int intensity = key.LOD / 3 + 1;

		int xElement = 0;
		int yElement = 0;
		switch( element )
		{
		case 0:
			xElement = 1;
			yElement = 2;
			break;

		case 1:
			xElement = 0;
			yElement = 2;
			break;

		case 2:
			xElement = 0;
			yElement = 1;
			break;
		}

		colour[xElement] = std::abs( std::fmod( key.X, 255.f ) ) / 255.f;
		colour[yElement] = std::abs( std::fmod( key.Y, 255.f ) ) / 255.f;
		colour[element] = 1.0f / intensity;

		return colour;
	}

	TerrainSystem::TerrainSystem( JobSystem& jobSystem ) :
		ddc::System( "Terrain System" ),
		ddr::Renderer( "Terrain" ),
		m_jobSystem( jobSystem ),
		m_previousOffset( INT_MAX, INT_MAX )
	{
		RequireWrite<TerrainChunkComponent>();
		RequireWrite<MeshComponent>();
		RequireWrite<BoundsComponent>();
		RequireWrite<TransformComponent>();

		Require<TerrainChunkComponent>();
		Require<MeshComponent>();

		SetPartitions( 1 );
	}

	TerrainSystem::~TerrainSystem()
	{
		
	}

	void TerrainSystem::Shutdown( ddc::World& world )
	{
		DestroyChunks( world );
	}

	void TerrainSystem::SetLODLevels( int lods )
	{
		m_lodLevels = lods;
		m_requiresRegeneration = true;
	}

	void TerrainSystem::RenderInit()
	{
		TerrainChunk::CreateRenderResources();
	}

	void TerrainSystem::Initialize( ddc::World& world )
	{
		TerrainChunk::InitializeShared();
	}

	void TerrainSystem::Render( const ddr::RenderData& data )
	{
		ddr::UniformStorage& uniforms = data.Uniforms();

		for( int i = 0; i < m_params.HeightLevelCount; ++i )
		{
			uniforms.Set( ddr::GetArrayUniformName( "TerrainHeightLevels", i, "Colour" ).c_str(), m_params.HeightColours[ i ] );
			uniforms.Set( ddr::GetArrayUniformName( "TerrainHeightLevels", i, "Cutoff" ).c_str(), m_params.HeightCutoffs[ i ] );
		}

		uniforms.Set( "TerrainHeightCount", m_params.HeightLevelCount );
		uniforms.Set( "TerrainMaxHeight", m_params.HeightRange );

		ddr::RenderBuffer<TerrainChunkComponent> chunks = data.Get<TerrainChunkComponent>();
		ddr::RenderBuffer<MeshComponent> meshes = data.Get<MeshComponent>();
		dd::Span<ddc::Entity> entities = data.Entities();

		for( size_t i = 0; i < entities.Size(); ++i )
		{
			chunks[ i ].Chunk->RenderUpdate( data.Uniforms() );
		}
	}

	void TerrainSystem::Update( const ddc::UpdateData& data, float delta_t )
	{
		ddc::World& world = data.World();

		if( m_requiresRegeneration )
		{
			DestroyChunks( world );

			m_requiresRegeneration = false;
		}

		if( m_saveChunkImages )
		{
			SaveChunkImages( world );

			m_saveChunkImages = false;
		}

		DD_TODO( "Introduce Camera Component and use that here." );
		glm::vec3 pos( 0.0f, 0.0f, 0.0f );
		float chunk_size = TerrainChunk::Vertices * m_params.VertexDistance;

		glm::ivec2 origin( (int) (pos.x / chunk_size), (int) (pos.z / chunk_size) );
		if( m_previousOffset != origin )
		{
			GenerateTerrain( data, origin );

			m_previousOffset = origin;
		}

		dd::Buffer<TerrainChunkComponent> chunks = data.Write<TerrainChunkComponent>();
		dd::Buffer<TransformComponent> transforms = data.Write<TransformComponent>();
		dd::Buffer<BoundsComponent> bounds = data.Write<BoundsComponent>();
		dd::Buffer<MeshComponent> meshes = data.Write<MeshComponent>();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			UpdateChunk( chunks[ i ], meshes[ i ], bounds[ i ], transforms[ i ] );
		}
	}

	void TerrainSystem::UpdateChunk( TerrainChunkComponent& chunk_cmp, MeshComponent& mesh_cmp, 
		BoundsComponent& bounds_cmp, TransformComponent& transform_cmp )
	{
		if( m_params.UseDebugColours )
		{
			mesh_cmp.Colour = GetMeshColour( chunk_cmp.Chunk->GetKey() );
		}
		else
		{
			mesh_cmp.Colour = glm::vec4( 1, 1, 1, 1 );
		}

		chunk_cmp.Chunk->Update( m_jobSystem );
		mesh_cmp.Mesh = chunk_cmp.Chunk->GetMesh();
		bounds_cmp.Local = chunk_cmp.Chunk->GetBounds();
		transform_cmp.SetLocalPosition( chunk_cmp.Chunk->GetPosition() );
	}

	void TerrainSystem::GenerateTerrain( const ddc::UpdateData& data, const glm::ivec2 offset )
	{
		const int expected = ChunksPerDimension * ChunksPerDimension + // lod 0
			(m_lodLevels - 1) * (ChunksPerDimension * ChunksPerDimension - (ChunksPerDimension / 2) * (ChunksPerDimension / 2)); // rest of the LODs

		Vector<TerrainChunkKey> required_chunks;
		required_chunks.Reserve( expected );

		// start with a 4x4 grid of the lowest LOD level, 
		// then at each level generate the centermost 2x2 grid into a 4x4 grid of one LOD level lower
		for( int lod = 0; lod < m_lodLevels; ++lod )
		{
			GenerateLODLevel( lod, required_chunks, offset );
		}

		UpdateTerrainChunks( data, required_chunks );
	}

	void TerrainSystem::UpdateTerrainChunks( const ddc::UpdateData& data, const Vector<TerrainChunkKey>& required_chunks )
	{
		ddc::World& world = data.World();

		dd::Buffer<MeshComponent> meshes = data.Write<MeshComponent>();
		dd::Buffer<TerrainChunkComponent> chunks = data.Write<TerrainChunkComponent>();

		dd::Span<ddc::Entity> entities = data.Entities();

		m_existing.clear();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			world.RemoveTag( entities[ i ], ddc::Tag::Visible );

			m_existing.insert( std::make_pair( chunks[ i ].Chunk->GetKey(), entities[ i ] ) );
		}

		Vector<TerrainChunkKey> missing_chunks;
		missing_chunks.Reserve( required_chunks.Size() );

		m_active.Clear();

		for( const TerrainChunkKey& required : required_chunks )
		{
			auto it = m_existing.find( required );
			if( it != m_existing.end() )
			{
				m_active.Add( it->second );
			}
			else
			{
				missing_chunks.Add( required );
			}
		}
		
		for( ddc::Entity entity : m_active )
		{
			world.AddTag( entity, ddc::Tag::Visible );
		}

		for( const TerrainChunkKey& key : missing_chunks )
		{
			ddc::Entity created = CreateChunk( world, key );
			m_existing.insert( std::make_pair( key, created ) );
		}
	}

	void TerrainSystem::GenerateLODLevel( int lod, Vector<TerrainChunkKey>& toGenerate, const glm::ivec2 offset )
	{
		DD_PROFILE_SCOPED( TerrainSystem_GenerateLODLevel );

		glm::ivec2 lod_offset( offset.x >> lod, offset.y >> lod );

		const float vertex_distance = m_params.VertexDistance * (1 << lod);
		const float chunk_size = TerrainChunk::Vertices * vertex_distance;

		const int halfChunks = ChunksPerDimension / 2;
		const int quarterChunks = halfChunks / 2;

		int generated = 0;

		for( int y = -halfChunks; y < halfChunks; ++y )
		{
			for( int x = -halfChunks; x < halfChunks; ++x )
			{
				// don't generate chunks for the middle-most grid unless we're at LOD 0
				if( lod != 0 &&
					(x >= -quarterChunks && x < quarterChunks) &&
					(y >= -quarterChunks && y < quarterChunks) )
				{
					continue;
				}

				TerrainChunkKey key;
				key.X = lod_offset.x * chunk_size + x * chunk_size;
				key.Y = lod_offset.y * chunk_size + y * chunk_size;
				key.LOD = lod;

				toGenerate.Add( key );

				++generated;
			}
		}

		const int expected = lod == 0 ? ChunksPerDimension * ChunksPerDimension : // lod 0
			(ChunksPerDimension * ChunksPerDimension - halfChunks * halfChunks); // rest of the LODs

		DD_ASSERT( expected == generated, "Wrong amount of chunks have been generated!\nExpected: %d\tActual: %d", expected, generated );
	}

	ddc::Entity TerrainSystem::CreateChunk( ddc::World& world, const TerrainChunkKey& key )
	{
		DD_PROFILE_SCOPED( TerrainSystem_CreateChunk );

		ddc::Entity& entity = world.CreateEntity<TransformComponent, MeshComponent, TerrainChunkComponent, BoundsComponent>();
		world.AddTag( entity, ddc::Tag::Visible );

		TransformComponent* transform_cmp = world.Access<TransformComponent>( entity );
		transform_cmp->SetLocalPosition( glm::vec3( key.X, 0, key.Y ) );

		TerrainChunkComponent* chunk_cmp = world.Access<TerrainChunkComponent>( entity );
		TerrainChunk* chunk = new TerrainChunk( m_params, key );
		chunk_cmp->Chunk = chunk;

		chunk->Generate();
		chunk->Update( m_jobSystem );

		return entity;
	}

	void TerrainSystem::DestroyChunks( ddc::World& world )
	{
		world.ForAllWith<TerrainChunkComponent>( [&world]( ddc::Entity entity, TerrainChunkComponent& chunk )
		{
			delete chunk.Chunk;
			chunk.Chunk = nullptr;
			world.DestroyEntity( entity );
		} );
	}

	void TerrainSystem::SaveChunkImages( const ddc::World& world ) const
	{
		int chunk_index = 0;

		world.ForAllWith<TerrainChunkComponent>( [&chunk_index]( ddc::Entity& entity, TerrainChunkComponent& chunk )
		{
			String64 chunk_file;
			snprintf( chunk_file.data(), 64, "terrain_%d.tga", chunk_index );

			chunk.Chunk->WriteHeightImage( chunk_file.c_str() );

			String64 chunk_normal_file;
			snprintf( chunk_file.data(), 64, "terrain_%d_n.tga", chunk_index );

			chunk.Chunk->WriteNormalImage( chunk_normal_file.c_str() );

			++chunk_index;
		} );
	}

	void TerrainSystem::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::Value( "Chunks", (int) m_existing.size() );
		ImGui::Value( "Active", m_active.Size() );

		ImGui::Checkbox( "Debug Colours", &m_params.UseDebugColours );

		if( ImGui::DragInt( "LODs", &m_lodLevels, 1, 1, 10 ) )
		{
			SetLODLevels( m_lodLevels );
		}

		if( ImGui::DragFloat( "Vertex Distance", &m_params.VertexDistance, 0.05f, 0.01f, 2.0f ) )
		{
			m_requiresRegeneration = true;
		}

		if( ImGui::DragFloat( "Height Range", &m_params.HeightRange, 1.0f, 0.0f, 200.0f ) )
		{
			m_requiresRegeneration = true;
		}

		if( ImGui::DragFloat( "Wavelength", &m_params.Wavelength, 1.0f, 1.0f, 512.0f ) )
		{
			m_requiresRegeneration = true;
		}

		if( ImGui::DragFloat( "Seed", &m_params.Seed, 0.1f, 0.0f, 512.0f ) )
		{
			m_requiresRegeneration = true;
		}

		if( ImGui::TreeNodeEx( "Height Colours", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			for( int i = 0; i < m_params.HeightLevelCount; ++i )
			{
				char name[ 64 ];
				snprintf( name, 64, "Height %d", i );

				if( ImGui::TreeNodeEx( name, ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					ImGui::ColorEdit3( "Colour", glm::value_ptr( m_params.HeightColours[i] ) );
					ImGui::DragFloat( "Cutoff", &m_params.HeightCutoffs[ i ], 0.01f, 0.0f, 1.0f );

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Amplitudes", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			for( int i = 0; i < m_params.Octaves; ++i )
			{
				char name[64];
				snprintf( name, 64, "Amplitude %d", i );

				if( ImGui::DragFloat( name, &m_params.Amplitudes[i], 0.001f, 0.0f, 1.0f ) )
				{
					m_requiresRegeneration = true;
				}
			}

			ImGui::TreePop();
		}

		if( ImGui::Button( "Randomize" ) )
		{
			RandomFloat rng( 0.0f, 1.0f );

			m_params.Seed = glm::mix( 0.0f, 512.0f, rng.Next() );
			m_params.HeightRange = glm::mix( 0.0f, 200.0f, rng.Next() );
			m_params.Wavelength = glm::mix( 1.0f, 512.0f, rng.Next() );

			float max_amplitude = 1.0f;
			for( int i = 0; i < m_params.Octaves; ++i )
			{
				float amplitude = glm::mix( 0.01f, max_amplitude, rng.Next() );

				m_params.Amplitudes[i] = amplitude;

				max_amplitude = amplitude;
			}

			for( int i = 0; i < m_params.HeightLevelCount; ++i )
			{
				m_params.HeightColours[i] = glm::vec3( rng.Next(), rng.Next(), rng.Next() );
			}

			float previous_cutoff = 0.0f;
			for( int i = 1; i < m_params.HeightLevelCount - 1; ++i )
			{
				float cutoff = glm::mix( previous_cutoff, 1.0f, rng.Next() );

				m_params.HeightCutoffs[i] = cutoff;

				previous_cutoff = cutoff;
			}

			m_requiresRegeneration = true;
		}
		
		if( ImGui::Button( "Save Chunk Heightmaps" ) )
		{
			m_saveChunkImages = true;
		}
	}
}
