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
		TOGGLE_BOUNDS,
		TOGGLE_PICKING,
		ADD_MINOR_TRAUMA,
		ADD_MAJOR_TRAUMA,
		BREAK,
		START_PARTICLE,
		CAMERA_POS_1, 
		CAMERA_POS_2,
		CAMERA_POS_3,
		CAMERA_POS_4,
		DECREASE_DEPTH,
		INCREASE_DEPTH,
		RESET_PHYSICS
	};

	enum class InputType : unsigned int
	{
		NONE,
		PRESSED,
		RELEASED
	};
}