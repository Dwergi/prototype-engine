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
		static const int Vertices = 4;

		static float HeightRange;
		static float Amplitudes[Octaves];
		static float Wavelength;

		static void GenerateSharedResources();

		TerrainChunk( const TerrainChunkKey& key );
		~TerrainChunk();
		
		void Generate( EntityManager& entityManager );
		void Destroy( EntityManager& entityManager );

		void Write( const char* filename );

	private:

		static const int IndexCount = Vertices * Vertices * 6;
		static const int VertexCount = (Vertices + 1) * (Vertices + 1);

		static uint s_indices[IndexCount];
		static ShaderHandle s_shader;

		EntityHandle m_entity;
		TerrainChunkKey m_key;
		glm::vec3 m_vertices[VertexCount];

		float GetHeight( float x, float y );
	};
}
