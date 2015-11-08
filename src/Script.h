//
// Script.h - Wrapper around an AngelScript script.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "SharedString.h"

namespace dd
{
	class Script
	{
	public:

		Script();
		Script( const Script& other );
		Script( const char* script );
		~Script();

	private:

		SharedString m_text;
	};
}