//
// FrustumTests.cpp - Tests for Frustum.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "Frustum.h"
#include "Camera.h"

TEST_CASE( "[Frustum]" )
{
	dd::Camera cam;
	cam.SetAspectRatio( 16, 9 );
	cam.SetDirection( glm::vec3( 0, 0, 1 ) );
	cam.SetPosition( glm::vec3( 0, 0, 0 ) );
	cam.SetFar( 100 );
	cam.SetNear( 1 );
	cam.SetVerticalFOV( glm::radians( 90.f / 2 ) );

	dd::Frustum f( cam );
}