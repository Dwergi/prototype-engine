//
// Memory.h - Helpers for memory management.
// Copyright (C) Sebastian Nordgren
// June 2nd 2016
//

#include "PCH.h"
#include "Memory.h"

static dd::ProfilerValue* g_allocs;
static dd::ProfilerValue* g_deletions;

namespace dd
{
	void InitializeMemoryTracking()
	{
		g_allocs = &dd::Profiler::GetValue("Memory/Allocations");
		g_deletions = &dd::Profiler::GetValue("Memory/Deletions");
	}
}

void* operator new(size_t size)
{
	if (g_allocs != nullptr)
	{
		g_allocs->Increment();
	}
	return std::malloc(size);
}

void* operator new[](size_t size)
{
	if (g_allocs != nullptr)
	{
		g_allocs->Increment();
	}
	return std::malloc(size);
}

void operator delete(void* ptr) noexcept
{
	if (g_deletions != nullptr)
	{
		g_deletions->Increment();
	}
	std::free(ptr);
}

void operator delete[](void* ptr) noexcept
{
	if (g_deletions != nullptr)
	{
		g_deletions->Increment();
	}
	std::free(ptr);
}