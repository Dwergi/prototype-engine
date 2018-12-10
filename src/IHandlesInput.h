//
// IHandlesInput.h
// Copyright (C) Sebastian Nordgren 
// December 9th 2018
//

#pragma once

#include "InputBindings.h"

namespace dd
{
	struct IHandlesInput
	{
		virtual void BindActions( InputBindings& bindings ) = 0;
	};
}