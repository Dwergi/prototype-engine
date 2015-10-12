//
// InputAction.h - Types of input actions that can be bound.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	enum class InputAction : unsigned int
	{
		NONE,
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		SHOOT,
		BOOST,
		CONSOLE
	};

	enum class InputType : unsigned int
	{
		NONE,
		PRESSED,
		RELEASED
	};
}