//
// Memory.h - Helpers for memory management.
// Copyright (C) Sebastian Nordgren
// June 2nd 2016
//

#pragma once

#define ALIGNED_ALLOCATORS( Align ) \
void* operator new( size_t sz ) { return _aligned_malloc( sz, Align ); } \
void* operator new(size_t sz, void* ptr) { return ptr; } \
void operator delete(void* p) {	_aligned_free( p ); } \
void operator delete(void* ptr, void* place) {}