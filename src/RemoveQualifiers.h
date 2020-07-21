//
// RemoveQualifiers.h - Gets the value type of a variable, minus references, pointers and class membership.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
// Derived from Randy Gaul (RandyGaul.net)
//

#pragma once

namespace dd
{
	template <typename T>
	struct StripMemberness;

	template <typename T, typename U>
	struct StripMemberness<T U::*>
	{
		typedef T type;
	};
}