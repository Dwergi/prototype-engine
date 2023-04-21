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

#include "ppk/ppk_assert.h"

// assert with a warning level
#define DD_ASSERT PPK_ASSERT
#define DD_ASSERT_WARNING PPK_ASSERT_WARNING
#define DD_ASSERT_DEBUG PPK_ASSERT_DEBUG
#define DD_ASSERT_ERROR PPK_ASSERT_ERROR
#define DD_ASSERT_FATAL PPK_ASSERT_FATAL
#define DD_ASSERT_CUSTOM PPK_ASSERT_CUSTOM

// assert that a return value from a function is used before it exits scope (eg. returning ownership of a pointer)
#define DD_ASSERT_USED PPK_ASSERT_USED
#define DD_ASSERT_USED_WARNING PPK_ASSERT_USED_WARNING
#define DD_ASSERT_USED_DEBUG PPK_ASSERT_USED_DEBUG
#define DD_ASSERT_USED_ERROR PPK_ASSERT_USED_ERROR
#define DD_ASSERT_USED_FATAL PPK_ASSERT_USED_FATAL
#define DD_ASSERT_USED_CUSTOM PPK_ASSERT_USED_CUSTOM

#ifdef _DEBUG
#define DD_DIAGNOSTIC( ... ) fprintf( stdout, __VA_ARGS__ );
#else
#define DD_DIAGNOSTIC( ... )
#endif

namespace dd
{
	enum class AssertAction
	{
		None = ppk::assert::implementation::AssertAction::None,
		Abort = ppk::assert::implementation::AssertAction::Abort,
		Break = ppk::assert::implementation::AssertAction::Break,
		Ignore = ppk::assert::implementation::AssertAction::Ignore,
		IgnoreLine = ppk::assert::implementation::AssertAction::IgnoreLine,
		IgnoreAll = ppk::assert::implementation::AssertAction::IgnoreAll,
		Throw = ppk::assert::implementation::AssertAction::Throw
	};

	enum class AssertLevel
	{
		Warning = ppk::assert::implementation::AssertLevel::Warning,
		Debug = ppk::assert::implementation::AssertLevel::Debug,
		Error = ppk::assert::implementation::AssertLevel::Error,
		Fatal = ppk::assert::implementation::AssertLevel::Fatal
	};
}

namespace dd::Assert
{
	void Initialize();
	void Shutdown();
	void Check();
}