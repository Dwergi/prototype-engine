//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "AABB.h"
#include "MaterialHandle.h"
#include "MeshHandle.h"
#include "ShaderHandle.h"
#include "TerrainChunkKey.h"

namespace ddr
{
	struct ICamera;
	struct ShaderProgram;
	struct UniformStorage;
}

namespace dd
{
	struct JobSystem;

	struct TerrainParameters;

	class TerrainChunk
	{
	public:

		//
		// Maximum number of LODs.
		//
		static const int MaxLODs = 6;

		//
		// The number of vertices per dimension of the chunk.
		//
		static const int MaxVertices = 1 << MaxLODs;

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

		void Update( JobSystem& job_system );
		void RenderUpdate( ddr::UniformStorage& uniforms );

		void Destroy();

		void WriteHeightImage( const char* filename ) const;

		const TerrainChunkKey& GetKey() const { return m_key; }
		AABB GetBounds() const { return m_bounds; }
		glm::vec3 GetPosition() const { return m_position; }
		
		ddr::MeshHandle GetMesh() const { return m_mesh; }

	private:

		static const int MeshVertexCount = (MaxVertices + 1) * (MaxVertices + 1);
		static const int FlapVertexCount = (MaxVertices + 1) * 4;
		static const int TotalVertexCount = MeshVertexCount + FlapVertexCount;

		static std::vector<uint> s_indices[MaxLODs];
		static ConstBuffer<uint> s_indexBuffers[MaxLODs];

		static ddr::ShaderHandle s_shader;
		static ddr::MaterialHandle s_material;

		const TerrainParameters& m_terrainParams;
		TerrainChunkKey m_key;
		
		bool m_destroy { false };
		bool m_renderDirty { false };
		bool m_dataDirty { false };
		ddr::MeshHandle m_mesh;

		glm::vec3 m_position;
		AABB m_bounds;

		glm::vec3 m_vertices[ TotalVertexCount ];
		Buffer<glm::vec3> m_verticesBuffer;

		glm::vec2 m_noiseOffset;

		int m_minLod { MaxLODs }; // the best quality LOD that's already been generated
		
		float GetNoise( float x, float y );

		void UpdateVertices( glm::vec2 chunkPos );

		ddr::Mesh* CreateMesh( const TerrainChunkKey& key );
	};
}
