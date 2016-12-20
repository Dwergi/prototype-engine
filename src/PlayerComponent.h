//
// PlayerComponent.h - Component to indicate that this is a player.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "ComponentBase.h"
#include "DenseMapPool.h"

namespace dd
{
	class PlayerComponent : public ComponentBase
	{
	public:

		typedef DenseMapPool<PlayerComponent> Pool;

		PlayerComponent() {}
		virtual ~PlayerComponent() {}

		BEGIN_TYPE( PlayerComponent )
			PARENT( ComponentBase )
		END_TYPE
	};
}