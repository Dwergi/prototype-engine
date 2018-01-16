//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "ShaderProgram.h"
#include "Mesh.h"

namespace dd
{
	class ICamera;
	class ShaderProgram;

	struct TerrainChunkKey;
	struct TerrainParameters;

	class TerrainChunk
	{
	public:

		//
		// The number of vertices per dimension of the chunk.
		//
		static const int Vertices = 16;

		//
		// Initialize resources that are shared between all chunks.
		//
		static void InitializeShared();

		//
		// Create render resources for all chunks.
		//
		static void CreateRenderResources();

		TerrainChunk( const TerrainParameters& params );
		~TerrainChunk();
		
		void Generate( const TerrainChunkKey& key );
		void SetOrigin( const TerrainChunkKey& key, glm::vec2 origin );

		void RenderUpdate( const TerrainChunkKey& key );

		void Destroy();

		void Write( const char* filename );

		MeshHandle GetMesh() const { return m_mesh; }

	private:

		static const int MeshIndexCount = Vertices * Vertices * 6;
		static const int FlapIndexCount = Vertices * 6 * 4;
		static const int IndexCount = MeshIndexCount + FlapIndexCount;

		static const int MeshVertexCount = (Vertices + 1) * (Vertices + 1);
		static const int FlapVertexCount = (Vertices + 1) * 4;
		static const int VertexCount = MeshVertexCount + FlapVertexCount;

		static uint s_indices[IndexCount];

		static ShaderHandle s_shader;

		const TerrainParameters& m_params;
		
		bool m_destroy { false };
		bool m_dirty { false };
		MeshHandle m_mesh;
		Buffer<glm::vec3> m_vertices;
		Buffer<uint> m_indices;
		
		float GetHeight( float x, float y );

		void UpdateVertices( const TerrainChunkKey& key, const glm::vec2& chunkPos );

		void CreateMesh( const TerrainChunkKey& key );
	};
}
