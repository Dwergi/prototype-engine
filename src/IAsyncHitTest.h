//
// IAsyncHitTest.h - Interface for anything that implements asynchronous hit testing.
// Copyright (C) Sebastian Nordgren 
// September 17th 2017
//

#pragma once

#include "HitState.h"

namespace dd
{
	struct IAsyncHitTest
	{
		// 
		// Start a hit test against the world with the given ray.
		// Length is the maximum length that will be considered.
		// Fetch results on the next frame after checking Completed().
		//
		virtual const HitState& ScheduleHitTest( const Ray& ray, float length ) = 0;
	};
}