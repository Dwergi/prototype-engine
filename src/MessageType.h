//
// MessageType.h - A base list of message types. 
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

namespace dd
{
	enum class MessageType : uint
	{
		Unknown = ~0u,
		EntityDestroyed = 0,
		EntityCreated,
		BulletHit
	};
}