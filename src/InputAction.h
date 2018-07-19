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
		EXIT,
		SELECT_MESH,
		TOGGLE_DEBUG_UI,
		TOGGLE_CONSOLE,
		TOGGLE_FREECAM,
		ADD_MINOR_TRAUMA,
		ADD_MAJOR_TRAUMA,
		TOGGLE_PICKING,
		BREAK,
		START_PARTICLE
	};

	enum class InputType : unsigned int
	{
		NONE,
		PRESSED,
		RELEASED
	};
}