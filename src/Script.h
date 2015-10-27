//
// Script.h - Wrapper around an AngelScript script.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	class Script
	{
	public:

		Script();
		Script( const char* script );
		~Script();

	private:

		String256 m_text;
	};
}