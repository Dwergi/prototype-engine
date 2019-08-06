//
// PrecompiledHeader.cpp
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
#include "imgui/imgui.h"

namespace dd
{
	void* PointerAdd( void* base, uint64 offset )
	{
		return (void*) ((size_t) base + offset);
	}

	const void* PointerAdd( const void* base, uint64 offset )
	{
		return (const void*) ((size_t) base + offset);
	}

#ifdef _MSC_VER
	bool DebuggerAttached()
	{
		return IsDebuggerPresent();
	}
#else
	bool DebuggerAttached()
	{
		return false;
	}
#endif
}