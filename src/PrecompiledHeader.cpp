#include "PrecompiledHeader.h"

bool operator==( const std::string& a, const std::string& b )
{
	return std::strcmp( a.c_str(), b.c_str() ) == 0;
}