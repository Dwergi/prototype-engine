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

		//
		// The number of octaves of noise.
		//
		static const int Octaves = 6;

		//
		// The number of vertices per dimension of the chunk.
		//
		static const int Vertices = 16;

		//
		// The number of height levels for colouring.
		//
		static const int HeightLevelCount = 5;

		//
		// Distance between vertices on the chunk.
		//
		static float VertexDistance;

		//
		// The maximum height range of vertices. Heights will vary between 0 and this.
		//
		static float HeightRange;

		//
		// The wavelength of the noise. 
		// Smaller numbers lead to higher frequency noise and more jagged terrain.
		// Larger numbers lead to smooth rolling hills.
		//
		static float Wavelength;

		//
		// A seed for noise generation to introduce some variation.
		//
		static float Seed;

		//
		// The amplitudes of noise to apply at each octave.
		//
		static float Amplitudes[Octaves];

		//
		// The colours to display at each height level.
		//
		static glm::vec3 HeightColours[HeightLevelCount];

		//
		// The cutoff points for the heights, expressed as a fraction of the maximum height range.
		//
		static float HeightCutoffs[ HeightLevelCount ];

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
		
		bool m_destroy { false };
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
