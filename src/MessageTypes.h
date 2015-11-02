//
// MessageTypes.h - A base list of message types. 
// Because these are passed by underlying value, it can be extended as desired.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

namespace dd
{
	enum class MessageTypes : uint
	{
		Unknown = (uint) -1,
		EntityDestroyed = 0,
		EntityCreated,
	};
}