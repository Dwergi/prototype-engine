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

#include <utility>
#include <iostream>
#include <algorithm>

#include "glm/fwd.hpp"
#include "glm/glm.hpp"

#include "DDAssert.h"
#include "Typedefs.h"
#include "TypeInfoMacros.h"

#include "Buffer.h"
#include "String_dd.h"
#include "Array.h"
#include "Vector.h"

#include "Variable.h"
#include "Member.h"
#include "FunctionPtr.h"
#include "FunctionSignature.h"
#include "Function.h"

#include "AutoList.h"
#include "DenseMap.h"

#include "TypeInfoHelpers.h"
#include "TypeInfo.h"

#include "Containers.h"

#include "Services.h"
#include "Globals.h"

#include "Serialization.h"

#include "ScriptEngine.h"