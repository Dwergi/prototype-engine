//
// Script.h - Wrapper around an AngelScript script.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Script.h"

namespace dd
{
	Script::Script()
	{

	}

	Script::Script( const Script& other )
		: m_text( other.m_text )
	{

	}

	Script::Script( const char* script )
		: m_text( script )
	{

	}

	Script::~Script()
	{

	}
}