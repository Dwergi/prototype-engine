//
// PlayerComponent.h - Component to indicate that this is a player.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Component.h"
#include "DenseMapPool.h"

namespace dd
{
	class PlayerComponent : public Component
	{
	public:

		typedef DenseMapPool<PlayerComponent> Pool;

		PlayerComponent() {}
		virtual ~PlayerComponent() {}

		BEGIN_TYPE( PlayerComponent )
			PARENT( Component );
		END_TYPE
	};
}