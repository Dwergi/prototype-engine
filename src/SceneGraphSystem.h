//
// SceneGraphSystem.h - A system that maintains a scene graph.
// Copyright (C) Sebastian Nordgren 
// April 22nd 2017
//

#pragma once

#include "ISystem.h"

namespace dd
{
	class SceneGraphSystem : public ISystem
	{
	public:
		virtual void PreUpdate( EntityManager& entity_manager, float dt ) override;
	};
}