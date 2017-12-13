//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "TerrainChunkKey.h"
#include "ShaderProgram.h"
#include "EntityHandle.h"

namespace dd
{
	class Camera;
	class ShaderProgram;

	class TerrainChunk
	{
	public:

		static const int Octaves = 8;
		static const int VerticesPerDim = 64 + 1;

		static float HeightRange;
		static float Amplitudes[Octaves];
		static float Wavelength;

		static void GenerateSharedResources();

		TerrainChunk( const TerrainChunkKey& key );
		~TerrainChunk();
		
		void Generate( EntityManager& entityManager );
		void Destroy( EntityManager& entityManager );

		void SetEnabled( bool enabled ) { m_enabled = enabled; }

		void Write( const char* filename );

	private:

		static int s_indices[(VerticesPerDim - 1) * (VerticesPerDim - 1) * 6];
		static ShaderHandle s_shader;

		int m_chunkSize;
		bool m_enabled;
		EntityHandle m_entity;

		TerrainChunkKey m_key;
		glm::vec3 m_vertices[VerticesPerDim * VerticesPerDim];

		float GetHeight( float x, float y );
	};
}
