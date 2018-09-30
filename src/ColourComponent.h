//
// ColourComponent.h - Component for storing an overall object colour.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#pragma once

namespace dd
{
	struct ColourComponent
	{
		glm::vec4 Colour;

		DD_CLASS( dd::ColourComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::ColourComponent, Colour );
		}
	};
}