//
// TrenchComponent.h - A single corridor of a trench.
// Copyright (C) Sebastian Nordgren 
// January 12th 2017
//

#pragma once

namespace dd
{
	class TrenchComponent
	{
	public:

		glm::vec3 Direction;

		DD_CLASS( dd::TrenchComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::TrenchComponent, Direction );
		}
	};
}