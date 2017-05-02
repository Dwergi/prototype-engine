//
// SceneGraphSystem.cpp - A system that maintains a scene graph.
// Copyright (C) Sebastian Nordgren 
// April 22nd 2017
//

#include "PrecompiledHeader.h"
#include "SceneGraphSystem.h"

#include "EntityManager.h"
#include "MeshComponent.h"
#include "TransformComponent.h"

namespace dd
{
	void SceneGraphSystem::PreUpdate( EntityManager& entity_manager, float dt )
	{
		entity_manager.ForAllWithWritable<TransformComponent>( [this]( auto entity, auto transform )
		{
			transform.Write()->UpdateWorldTransform();
		} );

		entity_manager.ForAllWithWritable<TransformComponent, MeshComponent>( [this]( auto entity, auto transform, auto mesh )
		{
			mesh.Write()->UpdateBounds( transform.Write()->GetWorldTransform() );
		} );
	}
}
