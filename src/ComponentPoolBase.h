//
// ComponentPoolBase.h - Base class for component pools.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "IComponent.h"
#include "ComponentHandle.h"
#include "EntityHandle.h"

namespace dd
{
	class ComponentPoolBase
	{
	public:

		//
		// Remove the component associated with the given entity.
		// 
		virtual void Remove( EntityHandle entity ) = 0;

		// 
		// Checks if the given entity has a component of this type.
		// 
		virtual bool Exists( EntityHandle entity ) const = 0;

	protected:

		ComponentPoolBase() {}
	};
}