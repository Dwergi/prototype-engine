//
// SceneGraphSystem.cpp - A system that maintains a scene graph.
// Copyright (C) Sebastian Nordgren 
// April 22nd 2017
//

#include "PrecompiledHeader.h"
#include "SceneGraphSystem.h"

#include "MeshComponent.h"
#include "TransformComponent.h"

namespace dd
{
	SceneGraphSystem::SceneGraphSystem() : 
		ddc::System( "Scene Graph" )
	{
		RequireWrite<dd::TransformComponent>();
	}

	void SceneGraphSystem::Update( const ddc::UpdateData& data, float dt )
	{
		for( dd::TransformComponent& transform : data.Write<dd::TransformComponent>() )
		{
			DD_TODO( "Apply parent transform" );
		}
	}
}
