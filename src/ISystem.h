//
// ISystem.h - Interface for a system.
// Copyright (C) Sebastian Nordgren 
// December 17th 2015
//

#pragma once

namespace dd
{
	class EntityManager;

	class ISystem
	{
	public:
		virtual void Update( EntityManager& entity_manager, float dt ) = 0;
	};
}