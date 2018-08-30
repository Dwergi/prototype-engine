//
// Message.h - Base struct for messages.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

namespace dd
{
	struct Message
	{
		uint Type;

		DD_BASIC_TYPE( Message )
	};
}