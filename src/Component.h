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

	public:
		virtual ~Component() {}

		BASIC_TYPE( Component )
	};
}