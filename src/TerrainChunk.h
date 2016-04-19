//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "VAO.h"
#include "VBO.h"

namespace dd
{
	class Camera;
	class ShaderProgram;

	struct ChunkKey
	{
		int64 X;
		int64 Y;
		int Size;
		int LOD;
	};

	template <>
	uint64 Hash( const ChunkKey& key );

	bool operator<( const ChunkKey& a, const ChunkKey& b );

	class TerrainChunk
	{
	public:

		static float HeightRange;
		static const int VerticesPerDim = 64;

		TerrainChunk( const ChunkKey& key );
		~TerrainChunk();

		void CreateRenderResources();

		void Generate();
		void Render( Camera& camera, ShaderProgram& shader );

		void Write( const char* filename );

	private:

		static VBO m_vboIndex;

		VAO m_vao;
		VBO m_vboVertex;

		ChunkKey m_key;
		glm::vec3 m_vertices[VerticesPerDim * VerticesPerDim];

		float GetHeight( float x, float y, float z );
	};
}
