//
// Component.h - Base component.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "EntityHandle.h"

namespace dd
{
	class Component
	{
	protected:
		Component() {}

	public:
		virtual ~Component() {}

		EntityHandle Entity;

		BEGIN_TYPE( Component )
			MEMBER( Component, Entity )
		END_TYPE
	};
}