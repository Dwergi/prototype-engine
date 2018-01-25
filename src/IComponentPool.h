//
// ComponentPoolBase.h - Base class for component pools.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "IComponent.h"

namespace dd
{
	class EntityHandle;

	class IComponentPool
	{
	public:
		//
		// Remove the component associated with the given entity.
		// 
		virtual void Remove( const EntityHandle& entity ) = 0;

		// 
		// Checks if the given entity has a component of this type.
		// 
		virtual bool Exists( const EntityHandle& entity ) const = 0;

	protected:

		IComponentPool() {}
	};
}