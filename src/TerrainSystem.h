//
// TerrainSystem.h - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

namespace dd
{
	class Camera;
	class ShaderProgram;
	class TerrainChunk;

	class TerrainSystem
	{
	public:

		static const uint LowDetailChunkSize = 128;
		static const uint LODLevels = 4;
		static const uint LowDetailChunksPerDim = 4; // 4 per dimension
		static const uint ChunksToSplit = 4; // chunks to split at each LOD level

		TerrainSystem( Camera& camera );
		~TerrainSystem();

		//
		// Set the size in metres of each chunk.
		//
		void SetChunkSize( uint size );
		uint GetChunkSize() const { return m_chunkSize; }

		void Initialize();
		void Update( float delta_t );
		void Render( Camera& camera, ShaderProgram& shader );

		void SaveChunkImages() const;

	private:

		uint m_chunkSize;
		Camera& m_camera;
		Vector<TerrainChunk*> m_chunks;
	};
}
