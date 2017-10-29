//
// PrecompiledHeader.h
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define GLM_SWIZZLE 1

#include "glm/fwd.hpp"
#include "glm/glm.hpp"

#include "Memory.h"

#include "Profiler.h"

#include "DDAssert.h"
#include "Typedefs.h"
#include "TypeInfoMacros.h"

#include "Math_dd.h"
#include "Buffer.h"
#include "DenseMap.h"
#include "String_dd.h"
#include "SharedString.h"
#include "Array.h"
#include "Vector.h"

#include "Variable.h"
#include "Member.h"
#include "FunctionSignature.h"
#include "Function.h"

#include "AutoList.h"

#include "TypeInfoHelpers.h"
#include "TypeInfo.h"

#include "Globals.h"

//#include "Serialization.h"
#include "RefCounter.h"
#include "AngelScriptEngine.h"

namespace dd
{
	const double M_PI = 3.14159265358979323846;
	const double M_PI_2 = 1.57079632679489661923;
	const double M_1_PI = 0.318309886183790671538;

	void* PointerAdd( void* base, uint64 offset );
	const void* PointerAdd( const void* base, uint64 offset );
}