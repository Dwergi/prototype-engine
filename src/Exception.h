//
// Exception.h - Base exception type.
// Copyright (C) Sebastian Nordgren 
// August 9th 2015
//

#pragma once

#include "String_dd.h"

namespace dd
{
	class Exception
	{
	public:

		Exception( const char* message );
		Exception( const dd::StringBase& message );

		dd::String256 Message;
	};
}
