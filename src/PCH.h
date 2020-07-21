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

#define GLM_FORCE_SWIZZLE 1

#pragma warning( disable : 4723 )

#include <algorithm>
#include <atomic>
#include <bitset>
#include <chrono>
#include <cmath>
#include <ctime>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <random>
#include <thread>
#include <unordered_map>
#include <vector>

#include "glm/fwd.hpp"
#include "glm/glm.hpp"

#include "Memory.h"

#include "Profiler.h"

#include "DDAssert.h"
#include "Typedefs.h"
#include "TypeInfoMacros.h"

#include "Math_dd.h"
#include "Buffer.h"
#include "String_dd.h"
#include "Array.h"
#include "Vector.h"
#include "Span.h"

#include "DenseMap.h"
#include "SharedString.h"

#include "Variable.h"
#include "Member.h"
#include "FunctionSignature.h"
#include "Function.h"

#include "AutoList.h"

#include "TypeInfoHelpers.h"
#include "TypeInfo.h"

#include "Globals.h"

#include "EntityLayer.h"

//#include "Serialization.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include "Services.h"

namespace dd
{
	const double M_PI = 3.14159265358979323846;
	const double M_PI_2 = 1.57079632679489661923;
	const double M_1_PI = 0.318309886183790671538;

	void* PointerAdd(void* base, uint64 offset);
	const void* PointerAdd(const void* base, uint64 offset);

	template <typename T, size_t Size>
	size_t ArrayLength(const T(&arr)[Size])
	{
		return Size;
	}

	template <typename T>
	T pop_front(std::vector<T>& vec)
	{
		T value = vec.front();
		vec.erase(vec.begin());

		return std::move(value);
	}

	template <typename T, typename Arg>
	void push_front(std::vector<T>& vec, Arg&& value)
	{
		vec.insert(vec.begin(), std::forward<Arg>(value));
	}

	template <typename T>
	const T& last(const std::vector<T>& vec)
	{
		DD_ASSERT(vec.size() > 0);
		return vec[vec.size() - 1];
	}

	template <typename T>
	T& last(std::vector<T>& vec)
	{
		DD_ASSERT(vec.size() > 0);
		return vec[vec.size() - 1];
	}

	bool DebuggerAttached();

	void SetAsMainThread();
	bool IsMainThread();
}

std::string operator+(std::string_view a, std::string_view b);

#define DD_TOSTRING2( X ) #X
#define DD_TOSTRING( X ) DD_TOSTRING2( X )
#define DD_TODO( Message ) __pragma(message( __FILE__ ":" DD_TOSTRING( __LINE__ ) " - TODO: " Message))

#define DD_OPTIMIZE_OFF() __pragma(optimize("",off))
#define DD_OPTIMIZE_ON() __pragma(optimize("",on))