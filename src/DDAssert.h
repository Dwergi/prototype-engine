//
// Assert.h - Wrappers around PPK assert macros.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "PP_Assert.h"

// assert with a warning level
#define ASSERT PPK_ASSERT
#define ASSERT_WARNING PPK_ASSERT_WARNING
#define ASSERT_DEBUG PPK_ASSERT_DEBUG
#define ASSERT_ERROR PPK_ASSERT_ERROR
#define ASSERT_FATAL PPK_ASSERT_FATAL
#define ASSERT_CUSTOM PPK_ASSERT_CUSTOM

// assert that a return value from a function is used before it exits scope (eg. returning ownership of a pointer)
#define ASSERT_USED PPK_ASSERT_USED
#define ASSERT_USED_WARNING PPK_ASSERT_USED_WARNING
#define ASSERT_USED_DEBUG PPK_ASSERT_USED_DEBUG
#define ASSERT_USED_ERROR PPK_ASSERT_USED_ERROR
#define ASSERT_USED_FATAL PPK_ASSERT_USED_FATAL
#define ASSERT_USED_CUSTOM PPK_ASSERT_USED_CUSTOM

#define STATIC_ASSERT PPK_STATIC_ASSERT