//
// Utility.cpp - A random selection of utility methods.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Utility.h"

bool operator==( const std::string& a, const std::string& b )
{
	return std::strcmp( a.c_str(), b.c_str() ) == 0;
}