//
// TrenchSystem.h - Procedural infinite trench system.
// Copyright (C) Sebastian Nordgren 
// January 12th 2017
//

#pragma once

#include "ISystem.h"
#include "EntityHandle.h"
#include "TransformComponent.h"
#include "TrenchComponent.h"

namespace dd
{
	class TrenchSystem : public ISystem
	{
	public: 

		TrenchSystem();
		~TrenchSystem();

		void Update( EntityManager& entity_manager, float delta_t ) override;

	private:

		// update a single trench component
		void UpdateComponent( EntityManager& entity_manager, EntityHandle handle, ComponentHandle<TrenchComponent> trench_comp, ComponentHandle<TransformComponent> transform_comp, glm::vec3 player_pos, glm::vec3 player_dir );
	};
}