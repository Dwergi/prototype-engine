#include "PCH.h"
#include "catch2/catch.hpp"

#include "Math_dd.h"

#include <glm/gtc/constants.hpp>

bool almostEqualRads( float a, float b )
{
	a = ddm::wrap( a, 0.0f, glm::pi<float>() );
	b = ddm::wrap( b, 0.0f, glm::pi<float>() );

	return abs( a - b ) < 0.001;
}

TEST_CASE( "[FPSCamera] Direction to Pitch/Yaw" )
{
	float pitch[] = { -85, -60, -30, 0, 30, 60, 85 };
	float yaw[] = { 0, 30, 60, 90, 120, 180, 210, 240, 270, 300, 330 };

	for( float p : pitch )
	{
		for( float y : yaw )
		{
			float pitch_rad = glm::radians( p );
			float yaw_rad = glm::radians( y );

			glm::vec3 dir = ddm::DirectionFromPitchYaw( pitch_rad, yaw_rad );

			float calc_pitch, calc_yaw;
			ddm::PitchYawFromDirection( dir, calc_pitch, calc_yaw );

			REQUIRE( almostEqualRads( calc_pitch, pitch_rad ) );
			REQUIRE( almostEqualRads( calc_yaw, yaw_rad ) );
		}
	}
}