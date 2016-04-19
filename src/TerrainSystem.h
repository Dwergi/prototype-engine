//
// TerrainSystem.h - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

namespace dd
{
	class Camera;
	class TerrainChunk;

	class TerrainSystem
	{
	public:

		static const int DefaultChunkSize = 16;
		static const int DefaultHighDetailChunks = 2;

		TerrainSystem( Camera& camera );
		~TerrainSystem();

		//
		// Set the number of high detail chunks generated close to the camera.
		// Must be a multiple of the chunk size or it doesn't make any sense!
		//
		void SetHighDetailChunks( int chunks );
		int SetHighDetailChunks() { return m_hdChunks; }

		//
		// Set the size in metres of each chunk.
		//
		void SetChunkSize( int size );
		int GetChunkSize() const { return m_chunkSize; }

		void Initialize();
		void Update( float delta_t );
		void Render();

	private:

		int m_hdChunks;
		int m_chunkSize;
		Camera& m_camera;
		Vector<TerrainChunk*> m_chunks;
	};
}
