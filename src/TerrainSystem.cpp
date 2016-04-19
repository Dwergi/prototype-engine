//
// TerrainSystem.cpp - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PrecompiledHeader.h"
#include "TerrainSystem.h"

#include "Camera.h"
#include "TerrainChunk.h"

namespace dd
{
	TerrainSystem::TerrainSystem( Camera& camera ) :
		m_camera( camera ),
		m_chunkSize( DefaultChunkSize ),
		m_hdChunks( DefaultHighDetailChunks )
	{

	}

	TerrainSystem::~TerrainSystem()
	{

	}

	void TerrainSystem::Initialize()
	{
		
	}

	void TerrainSystem::Update( float delta_t )
	{

	}

	void TerrainSystem::Render()
	{

	}

	void TerrainSystem::SetChunkSize( int size )
	{
		m_chunkSize = size;
	}

	void TerrainSystem::SetHighDetailChunks( int chunks )
	{
		m_hdChunks = chunks;
	}
}
