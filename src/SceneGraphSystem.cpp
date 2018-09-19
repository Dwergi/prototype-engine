//
// SceneGraphSystem.cpp - A system that maintains a scene graph.
// Copyright (C) Sebastian Nordgren 
// April 22nd 2017
//

#include "PrecompiledHeader.h"
#include "SceneGraphSystem.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "BoundsHelpers.h"
#include "TransformComponent.h"

namespace dd
{
	SceneGraphSystem::SceneGraphSystem() : 
		ddc::System( "Scene Graph" )
	{
		DD_TODO( "Delete!" );
	}

	void SceneGraphSystem::Update( const ddc::UpdateData& data )
	{
		
	}
}
