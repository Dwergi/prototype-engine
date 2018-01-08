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

	class TerrainChunk
	{
	public:

		static const int Octaves = 6;
		static const int Vertices = 16;

		static float VertexDistance;
		static float HeightRange;
		static float Amplitudes[Octaves];
		static float Wavelength;
		static bool UseDebugColours;

		static void InitializeShared();
		static void CreateRenderResources();

		TerrainChunk();
		~TerrainChunk();
		
		void Generate( const TerrainChunkKey& key );
		void SetOrigin( const TerrainChunkKey& key, glm::vec2 origin );

		void RenderUpdate( const TerrainChunkKey& key );

		void Destroy();

		void Write( const char* filename );
		void WriteNormals( const char* filename );

		MeshHandle GetMesh() const { return m_mesh; }

	private:

		static const int IndexCount = Vertices * Vertices * 6;
		static const int VertexCount = (Vertices + 1) * (Vertices + 1);

		static uint s_indices[IndexCount];
		static Buffer<uint> s_bufferIndices;

		static ShaderHandle s_shader;

		bool m_dirty { false };
		MeshHandle m_mesh;
		Buffer<glm::vec3> m_vertices;
		Buffer<glm::vec3> m_normals;
		
		float GetHeight( float x, float y );

		void UpdateVertices( const TerrainChunkKey& key, const glm::vec2& chunkPos );
		void UpdateNormals();

		void CreateMesh( const TerrainChunkKey& key );
	};
}
