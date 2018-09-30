//
// PlayerComponent.h - Component to indicate that this is a player.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	class PlayerComponent
	{
	public:

		PlayerComponent() {}
		virtual ~PlayerComponent() {}

		DD_CLASS( dd::PlayerComponent )
		{
			DD_COMPONENT();
		}
	};
}