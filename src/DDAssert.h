//
// Assert.h - Wrappers around PPK assert macros.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "PP_Assert.h"

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

#define DD_STATIC_ASSERT PPK_STATIC_ASSERT