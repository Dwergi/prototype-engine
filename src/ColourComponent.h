//
// ColourComponent.h - Component for storing an overall object colour.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#pragma once

#include "ComponentType.h"

namespace dd
{
	struct ColourComponent
	{
		glm::vec4 Colour;

		DD_COMPONENT;
	};
}