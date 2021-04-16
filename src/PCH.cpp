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

	int SplitString(const std::string& to_split, dd::IArray<std::string>& out_parts, char split_on)
	{
		size_t start = 0;
		size_t end = to_split.find(split_on);
		while (end != std::string::npos)
		{
			std::string part = to_split.substr(start, end - start);
			out_parts.Add(part);

			start = end + 1;
			end = to_split.find(split_on, start);
		}

		out_parts.Add(to_split.substr(start));
		return out_parts.Size();
	}

	static std::thread::id s_mainThread;

	void SetAsMainThread()
	{
		s_mainThread = std::this_thread::get_id();
	}

	bool IsMainThread()
	{
		return std::this_thread::get_id() == s_mainThread;
	}
}

std::string operator+(std::string_view a, std::string_view b)
{
	return std::string(a) + std::string(b);
}