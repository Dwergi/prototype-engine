//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "TerrainChunkKey.h"
#include "VAO.h"
#include "VBO.h"

#include <atomic>
#include <memory>

namespace dd
{
	class Camera;
	class ShaderProgram;

	class TerrainChunk
	{
	public:

		static float HeightRange;
		static const int VerticesPerDim = 64 + 1;

		TerrainChunk( const TerrainChunkKey& key );
		~TerrainChunk();

		void CreateRenderResources( ShaderProgram& shader );

		void Generate();
		void Render( Camera& camera );

		void Write( const char* filename );

	private:

		template<int Width, int Height>
		class IndexBuffer
		{
		public:
			static const uint Count = (Width - 1) * (Height - 1) * 6;

			uint16 Indices[Count];

			IndexBuffer();
			~IndexBuffer();
			void Create();
			void Bind();

		private:
			VBO m_vbo;
			std::atomic<bool> m_created;
		};

		static IndexBuffer<VerticesPerDim, VerticesPerDim> s_indices;

		std::atomic<bool> m_generated;
		std::atomic<bool> m_created;

		VAO m_vao;
		VBO m_vboVertex;
		ShaderProgram* m_shader;

		TerrainChunkKey m_key;
		glm::vec3 m_vertices[VerticesPerDim * VerticesPerDim];

		float GetHeight( float x, float y );
	};
}

#include "TerrainChunk.inl"
