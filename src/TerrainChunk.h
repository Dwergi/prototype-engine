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
	class ICamera;
	class ShaderProgram;

	class TerrainChunk
	{
	public:

		static const int Octaves = 8;
		static const int Vertices = 8;

		static float HeightRange;
		static float Amplitudes[Octaves];
		static float Wavelength;
		static bool UseDebugColours;

		static void GenerateSharedResources();

		TerrainChunk( const TerrainChunkKey& key );
		~TerrainChunk();
		
		void Generate( EntityManager& entityManager );
		void Update( glm::vec3& origin );
		void Destroy( EntityManager& entityManager );

		void Write( const char* filename );

	private:

		static const int IndexCount = Vertices * Vertices * 6;
		static const int VertexCount = (Vertices + 1) * (Vertices + 1);

		static uint s_indices[IndexCount];
		static ShaderHandle s_shader;

		EntityHandle m_entity;
		TerrainChunkKey m_key;
		glm::vec3 m_vertices[VertexCount * 2]; // vertices and normals
		glm::vec2 m_lastPosition;

		float GetHeight( float x, float y );

		void UpdateVertices( const glm::vec2& chunkPos );
		void UpdateNormals();
	};
}
