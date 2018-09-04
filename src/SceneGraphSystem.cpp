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
		ddc::WriteBuffer<dd::TransformComponent> transforms = data.Write<dd::TransformComponent>();

		for( dd::TransformComponent& transform : transforms )
		{
			transform.World = transform.Local;
		}
	}
}
