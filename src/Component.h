//
// Component.h - Base component.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	class Component
	{
	protected:
		Component() {}

		NO_MEMBERS( Component )

	public:
		virtual ~Component() {}
	};
}