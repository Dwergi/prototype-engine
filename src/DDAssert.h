//
// Assert.h - Wrappers around PPK assert macros.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#ifdef _DEBUG
#define PPK_ASSERT_ENABLED 1
#else 
#define PPK_ASSERT_ENABLED 1
#endif

#include "PP_Assert.h"

// assert with a warning level
#define DD_ASSERT PPK_ASSERT
#define DD_ASSERT_WARNING PPK_ASSERT_WARNING
#define DD_ASSERT_DEBUG PPK_ASSERT_DEBUG
#define DD_ASSERT_ERROR PPK_ASSERT_ERROR
#define DD_ASSERT_FATAL PPK_ASSERT_FATAL
#define DD_ASSERT_CUSTOM PPK_ASSERT_CUSTOM

#define DD_ENABLE_SLOW_ASSERTS 1

#if DD_ENABLE_SLOW_ASSERTS
#define DD_ASSERT_SLOW(...) DD_ASSERT(__VA_ARGS__)
#else
#define DD_ASSERT_SLOW(...)
#endif

// assert that a return value from a function is used before it exits scope (eg. returning ownership of a pointer)
#define DD_ASSERT_USED PPK_ASSERT_USED
#define DD_ASSERT_USED_WARNING PPK_ASSERT_USED_WARNING
#define DD_ASSERT_USED_DEBUG PPK_ASSERT_USED_DEBUG
#define DD_ASSERT_USED_ERROR PPK_ASSERT_USED_ERROR
#define DD_ASSERT_USED_FATAL PPK_ASSERT_USED_FATAL
#define DD_ASSERT_USED_CUSTOM PPK_ASSERT_USED_CUSTOM

#define DD_ENABLE_DIAGNOSTIC 0

#if DD_ENABLE_DIAGNOSTIC
#define DD_DIAGNOSTIC( ... ) { char buffer[256]; snprintf(buffer, 128, __VA_ARGS__); OutputDebugStringA(buffer); }
#else
#define DD_DIAGNOSTIC( ... )
#endif

namespace dd
{
	enum class AssertAction
	{
		None = pempek::assert::implementation::AssertAction::None,
		Abort = pempek::assert::implementation::AssertAction::Abort,
		Break = pempek::assert::implementation::AssertAction::Break,
		Ignore = pempek::assert::implementation::AssertAction::Ignore,
		IgnoreLine = pempek::assert::implementation::AssertAction::IgnoreLine,
		IgnoreAll = pempek::assert::implementation::AssertAction::IgnoreAll,
		Throw = pempek::assert::implementation::AssertAction::Throw
	};

	enum class AssertLevel
	{
		Warning = pempek::assert::implementation::AssertLevel::Warning,
		Debug = pempek::assert::implementation::AssertLevel::Debug,
		Error = pempek::assert::implementation::AssertLevel::Error,
		Fatal = pempek::assert::implementation::AssertLevel::Fatal
	};
}
