//
// TrenchSystem.cpp - Procedural infinite trench system.
// Copyright (C) Sebastian Nordgren 
// January 12th 2017
//

#include "PrecompiledHeader.h"
#include "TrenchSystem.h"

#include "EntityManager.h"
#include "PlayerComponent.h"
#include "TransformComponent.h"
#include "TrenchComponent.h"

const float TRENCH_CHUNK_LENGTH = 10.0f;
const float TRENCH_CHUNK_DESTRUCTION_DISTANCE = 100.0f;
const float TRENCH_CHUNK_CREATION_DISTANCE = 100.0f;

/*
DATA:

Each TrenchComponent:
- Represents a segment of trench TRENCH_CHUNK_LENGTH long.
- Is a cube, with 2 sides open
    - 4 of the sides are made up of meshes and potentially some gameplay component (turret, target, etc.)
	- 2 of the sides are open
	- In the case where there is a corner, the player is forced to turn

LOGIC:

On each update:
- Check if a chunk is too far (TRENCH_CHUNK_DESTRUCTION_DISTANCE) behind the player and destroy the entity if it is
- Create new chunks ahead of the player if necessary
*/

namespace dd
{
	TrenchSystem::TrenchSystem()
	{

	}

	TrenchSystem::~TrenchSystem()
	{

	}

	void TrenchSystem::Update( EntityManager& entity_manager, float delta_t )
	{
		Vector<EntityHandle> player_entity = entity_manager.FindAllWithReadable<PlayerComponent, TransformComponent>();

		const TransformComponent* player_transform = player_entity[ 0 ].Get<TransformComponent>().Read();

		// cache these here to avoid recalculating for all components
		glm::vec3 player_pos = player_transform->GetPosition();
		glm::vec3 player_dir = player_transform->GetDirection();
		
		entity_manager.ForAllWithReadable<TrenchComponent, TransformComponent>( [this, &entity_manager, player_pos, player_dir]
			( EntityHandle handle, ComponentHandle<TrenchComponent> trench_comp, ComponentHandle<TransformComponent> transform_comp ) 
			{ 
				UpdateComponent( entity_manager, handle, trench_comp, transform_comp, player_pos, player_dir );
			} );
	}

	void TrenchSystem::UpdateComponent( EntityManager& entity_manager, EntityHandle handle, ComponentHandle<TrenchComponent> trench_comp, ComponentHandle<TransformComponent> transform_comp, glm::vec3 player_pos, glm::vec3 player_dir )
	{
		glm::vec3 distance = player_pos - transform_comp.Read()->GetPosition();
		if( (distance / player_dir).length() < -TRENCH_CHUNK_DESTRUCTION_DISTANCE )
		{
			entity_manager.Destroy( handle );
		}
	}
}