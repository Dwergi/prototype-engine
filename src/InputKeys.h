//
// InputKeys.h
// Copyright (C) Sebastian Nordgren 
// August 2nd 2018
//

#pragma once

#include "EnumFlags.h"

namespace dd
{
	enum class Key : int16
	{
		NONE = -1,
		SPACE = 32,
		KEY_0 = 48,
		KEY_1 = 49,
		KEY_2 = 50,
		KEY_3 = 51,
		KEY_4 = 52,
		KEY_5 = 53,
		KEY_6 = 54,
		KEY_7 = 55,
		KEY_8 = 56,
		KEY_9 = 57,
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,
		ESCAPE = 256,
		ENTER = 257,
		TAB = 258,
		BACKSPACE = 259,
		INSERT = 260,
		DEL = 261,
		RIGHT_ARROW = 262,
		LEFT_ARROW = 263,
		DOWN_ARROW = 264,
		UP_ARROW = 265,
		PAGE_UP = 266,
		PAGE_DOWN = 267,
		HOME = 268,
		END = 269,
		PAUSE = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		LSHIFT = 340,
		LCTRL = 341,
		LALT = 342,

		KB_LAST = 511,
		MOUSE_LEFT = 512,
		MOUSE_RIGHT,
		MOUSE_MIDDLE,
		MOUSE_4,
		MOUSE_5
	};

	enum class Modifier : uint8
	{
		None = 0,
		Shift = 1 << 0,
		Ctrl = 1 << 1,
		Alt = 1 << 2
	};

	using ModifierFlags = EnumFlags<dd::Modifier>;
}