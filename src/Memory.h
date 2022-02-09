//
// Memory.h - Helpers for memory management.
// Copyright (C) Sebastian Nordgren
// June 2nd 2016
//

#pragma once

#define DD_ALIGNED_ALLOCATORS(Align) \
void* operator new(size_t size) { return _aligned_malloc(size, Align); } \
void* operator new[](size_t size) { return _aligned_malloc(size, Align); } \
void* operator new(size_t size, void* ptr) { return ptr; } \
void operator delete(void* ptr) { _aligned_free(ptr); } \
void operator delete[](void* ptr) { _aligned_free(ptr); } \
void operator delete(void* ptr, void* place) {}

[[nodiscard]]
void* operator new(size_t size);
void operator delete(void* ptr) noexcept;

[[nodiscard]]
void* operator new[](size_t size);
void operator delete[](void* ptr) noexcept;

namespace dd
{
	void InitializeMemoryTracking();

	void BreakOnAlloc(bool enable);
}