//
// Component.h - Base component.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "EntityHandle.h"

namespace dd
{
	class ComponentBase
	{
	protected:

		ComponentBase() {}
		ComponentBase( const ComponentBase& other ) : Entity( other.Entity ) {}

	public:
		virtual ~ComponentBase() {}

		EntityHandle Entity;

		BEGIN_TYPE( ComponentBase )
			MEMBER( ComponentBase, Entity )
		END_TYPE
	};
}