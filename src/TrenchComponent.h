//
// TrenchComponent.h - A single corridor of a trench.
// Copyright (C) Sebastian Nordgren 
// January 12th 2017
//

#pragma once

#include "ComponentType.h"

namespace dd
{
	class TrenchComponent
	{
	public:

		glm::vec3 Direction;

		TrenchComponent() { }

		DD_COMPONENT;

		DD_SCRIPT_OBJECT( TrenchComponent )
		DD_END_TYPE
	};
}