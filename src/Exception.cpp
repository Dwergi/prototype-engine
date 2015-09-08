//
// Exception.cpp - Base exception class.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Exception.h"

dd::Exception::Exception( const char* message )
{
	Message = message;
}

dd::Exception::Exception( const dd::String& message )
{
	Message = message;
}