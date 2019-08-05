//
// MessageType.h - A base list of message types. 
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

namespace ddc
{
	enum class MessageType : uint
	{
		Unknown = ~0u,
		Test = 0,
		EntityDestroyed,
		EntityCreated,
		BulletHit
	};
}