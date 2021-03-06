//
// PlayerComponent.h - Component to indicate that this is a player.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	struct PlayerComponent
	{
		DD_BEGIN_CLASS( dd::PlayerComponent )
			DD_COMPONENT();
		DD_END_CLASS()
	};
}