//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "VAO.h"
#include "VBO.h"

#include <atomic>
#include <memory>

namespace dd
{
	class Camera;
	class ShaderProgram;

	struct ChunkKey
	{
		ChunkKey() :
			X( 0xFFFFFFF ),
			Y( 0xFFFFFFF ),
			Size( 0 ),
			IsSplit( false ) {}

		int64 X;
		int64 Y;
		int Size;
		bool IsSplit;
	};

	template <>
	uint64 Hash( const ChunkKey& key );

	bool operator<( const ChunkKey& a, const ChunkKey& b );

	class TerrainChunk
	{
	public:

		static float HeightRange;
		static const int VerticesPerDim = 64 + 1;

		TerrainChunk( const ChunkKey& key );
		~TerrainChunk();

		void CreateRenderResources( ShaderProgram& shader );

		void Generate();
		void Render( Camera& camera );

		void Write( const char* filename );

	private:

		static VBO m_vboIndex;
		static std::mutex m_indexMutex;

		std::atomic<bool> m_generated;

		VAO m_vao;
		VBO m_vboVertex;
		ShaderProgram* m_shader;

		ChunkKey m_key;
		glm::vec3 m_vertices[VerticesPerDim * VerticesPerDim];

		float GetHeight( float x, float y );
	};
}
