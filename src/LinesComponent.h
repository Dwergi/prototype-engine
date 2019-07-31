//
// LinesComponent.h - Component for a set of lines.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#pragma once

namespace dd
{
	struct LinesComponent
	{
		dd::Vector<glm::vec3> Points;

		DD_BEGIN_CLASS( dd::LinesComponent )
			DD_COMPONENT();
		DD_END_CLASS()
	};
}
