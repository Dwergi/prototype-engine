//
// ISystem.h - Interface for a system.
// Copyright (C) Sebastian Nordgren 
// December 17th 2015
//

#pragma once

namespace dd
{
	class ISystem
	{
	public:
		virtual void Update( float dt ) = 0;
	};
}