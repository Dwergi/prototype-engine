//
// WaterSystem.cpp
// Copyright (C) Sebastian Nordgren 
// December 3rd 2018
//

#include "PCH.h"
#include "WaterSystem.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "TerrainChunk.h"
#include "TerrainChunkComponent.h"
#include "WaterComponent.h"

namespace dd
{
	WaterSystem::WaterSystem() :
		ddc::System( "Water" )
	{
		RequireRead<TerrainChunkComponent>();
		OptionalRead<BoundBoxComponent>();
		OptionalRead<BoundSphereComponent>();
		RequireWrite<WaterComponent>();
	}

	void WaterSystem::Update( const ddc::UpdateData& update_data )
	{

	}
}