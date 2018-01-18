//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "Mesh.h"
#include "ShaderProgram.h"
#include "TerrainChunkKey.h"

namespace dd
{
	class ICamera;
	class ShaderProgram;
	class JobSystem;

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

		TerrainChunk( const TerrainParameters& params, const TerrainChunkKey& key );
		~TerrainChunk();
		
		void Generate();
		void SetNoiseOffset( glm::vec2 origin );

		void Update( JobSystem& job_system, float delta_t );
		void RenderUpdate();

		void Destroy();

		void WriteHeightImage( const char* filename ) const;
		void WriteNormalImage( const char* filename ) const;

		const TerrainChunkKey& GetKey() const { return m_key; }
		
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
		TerrainChunkKey m_key;
		
		bool m_destroy { false };
		bool m_renderDirty { false };
		bool m_dataDirty { false };
		MeshHandle m_mesh;

		glm::vec3 m_vertices[VertexCount];
		Buffer<glm::vec3> m_verticesBuffer;

		glm::vec3 m_normals[VertexCount];
		Buffer<glm::vec3> m_normalsBuffer;
		Buffer<uint> m_indices;

		glm::vec2 m_noiseOffset;
		
		float GetHeight( float x, float y );

		void UpdateVertices( glm::vec2 chunkPos );
		void UpdateNormals();

		void CreateMesh( const TerrainChunkKey& key );
	};
}
