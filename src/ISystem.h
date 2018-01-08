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
		virtual void Initialize( EntityManager& entity_manager ) {}

		virtual void PreUpdate( EntityManager& entity_manager, float dt ) {}
		virtual void Update( EntityManager& entity_manager, float dt ) {}
		virtual void PostRender( EntityManager& entity_manager, float dt ) {}

		virtual void Shutdown( EntityManager& entity_manager ) {}
	};
}