//
// IAsyncHitTest.h - Interface for anything that implements asynchronous hit testing.
// Copyright (C) Sebastian Nordgren 
// September 17th 2017
//

#pragma once

#include "HitResult.h"

namespace dd
{
	struct IAsyncHitTest
	{
		// 
		// Start a hit test against the entities with the given ray.
		// Length is the maximum length that will be considered.
		// Fetch the current state using FetchResult().
		//
		virtual HitHandle ScheduleHitTest( const ddm::Ray& ray ) = 0;

		//
		// Fetch the hit state of a given hit.
		//
		virtual bool FetchResult( HitHandle handle, HitResult& result ) = 0;

		//
		// Release the state pointed to by the handle, and invalidate the handle.
		//
		virtual void ReleaseResult( HitHandle handle ) = 0;
	};
}