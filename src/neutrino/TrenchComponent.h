//
// TrenchComponent.h - A single corridor of a trench.
// Copyright (C) Sebastian Nordgren 
// January 12th 2017
//

#pragma once

namespace dd
{
	struct TrenchComponent
	{
	public:

		glm::vec3 Direction;

		DD_BEGIN_CLASS( dd::TrenchComponent )
			DD_COMPONENT();
			DD_MEMBER( Direction );
		DD_END_CLASS()
	};
}