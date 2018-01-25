//
// PlayerComponent.h - Component to indicate that this is a player.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "IComponent.h"
#include "PackedPool.h"

namespace dd
{
	class PlayerComponent : public IComponent
	{
	public:

		typedef PackedPool<PlayerComponent> Pool;

		PlayerComponent() {}
		virtual ~PlayerComponent() {}

		BEGIN_TYPE( PlayerComponent )
			PARENT( IComponent )
		END_TYPE
	};
}