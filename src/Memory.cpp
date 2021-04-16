//
// Memory.h - Helpers for memory management.
// Copyright (C) Sebastian Nordgren
// June 2nd 2016
//

#include "PCH.h"
#include "Memory.h"

static dd::ProfilerValue* g_allocs;
static dd::ProfilerValue* g_deletions;
static std::atomic<bool> g_breakOnAlloc = false;

namespace dd
{
	void InitializeMemoryTracking()
	{
		g_allocs = &dd::Profiler::GetValue("Memory/Allocations");
		g_deletions = &dd::Profiler::GetValue("Memory/Deletions");
	}

	void BreakOnAlloc(bool enable)
	{
		g_breakOnAlloc = enable;
	}
}

void* operator new(size_t size)
{
	if (g_allocs != nullptr)
	{
		g_allocs->Increment();
	}

	if (g_breakOnAlloc)
	{
		__debugbreak();
	}

	return std::malloc(size);
}

void* operator new[](size_t size)
{
	if (g_allocs != nullptr)
	{
		g_allocs->Increment();
	}

	if (g_breakOnAlloc)
	{
		__debugbreak();
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