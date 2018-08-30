//
// PlayerComponent.h - Component to indicate that this is a player.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "ComponentType.h"

namespace dd
{
	class PlayerComponent
	{
	public:

		PlayerComponent() {}
		virtual ~PlayerComponent() {}

		DD_COMPONENT;

		DD_BEGIN_TYPE( PlayerComponent )
		DD_END_TYPE
	};
}