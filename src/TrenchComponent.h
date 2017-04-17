//
// TrenchComponent.h - A single corridor of a trench.
// Copyright (C) Sebastian Nordgren 
// January 12th 2017
//

#pragma once

#include "ComponentBase.h"
#include "DenseMapPool.h"

namespace dd
{
	class TrenchComponent : public ComponentBase
	{
	public:

		glm::vec3 Direction;

		typedef DenseMapPool<TrenchComponent> Pool;

		TrenchComponent() { }

		BEGIN_SCRIPT_OBJECT( TrenchComponent )
			PARENT( ComponentBase )
		END_TYPE
	};
}