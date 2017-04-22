//
// SceneGraphSystem.cpp - A system that maintains a scene graph.
// Copyright (C) Sebastian Nordgren 
// April 22nd 2017
//

#include "PrecompiledHeader.h"
#include "SceneGraphSystem.h"

#include "EntityManager.h"
#include "TransformComponent.h"

namespace dd
{
	void SceneGraphSystem::PreUpdate( EntityManager& entity_manager, float dt )
	{
		entity_manager.ForAllWithReadable<TransformComponent>( [this]( EntityHandle entity, ComponentHandle<TransformComponent> transform )
		{
			transform.Write()->UpdateWorldTransform();
		} );
	}
}
