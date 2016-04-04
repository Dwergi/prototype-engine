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

// min & max, mostly
#include <algorithm>

#define GLM_SWIZZLE 1

#include "glm/fwd.hpp"
#include "glm/glm.hpp"

#include "Profiler.h"

#include "DDAssert.h"
#include "Typedefs.h"
#include "TypeInfoMacros.h"

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

#include "Services.h"
#include "Globals.h"

#include "Serialization.h"
#include "RefCounter.h"

#ifdef USE_ANGELSCRIPT

#include "AngelScriptEngine.h"
namespace dd
{
	typedef AngelScriptEngine ScriptEngine;
}

#else

#include "WrenEngine.h"
namespace dd
{
	typedef WrenEngine ScriptEngine;
}
	
#endif 
