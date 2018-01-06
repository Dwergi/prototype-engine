//
// Component.h - Base component.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "EntityHandle.h"

namespace dd
{
	class IComponent
	{
	protected:

		IComponent() {}
		IComponent( const IComponent& other ) : Entity( other.Entity ) {}

	public:
		virtual ~IComponent() {}

		EntityHandle Entity;

		BEGIN_TYPE( IComponent )
			MEMBER( IComponent, Entity )
		END_TYPE
	};
}