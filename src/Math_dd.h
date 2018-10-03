//
// Math_dd.h - Additional maths functions one might need.
// Copyright (C) Sebastian Nordgren 
// October 25th 2017
//

#pragma once

namespace dd
{
	struct Ray;
}

namespace ddm
{
#undef min
#undef max

	template <typename T>
	T min( T a, T b )
	{
		return a <= b ? a : b;
	}

	template <typename T>
	T min( T a, T b, T c )
	{
		return ddm::min( a, ddm::min( b, c ) );
	}

	template <typename T>
	T max( T a, T b )
	{
		return a >= b ? a : b;
	}

	template <typename T>
	T max( T a, T b, T c )
	{
		return ddm::max( a, ddm::max( b, c ) );
	}

	template <typename T>
	T wrap( T value, T min, T max )
	{
		value = min + std::fmod( value - min, max - min );

		if( value < 0 )
			value += max;

		return value;
	}

	bool IsNaN( glm::vec3 v );
	bool IsInf( glm::vec3 v );

	glm::mat4 TransformFromOriginDir( const glm::vec3& origin, const glm::vec3& direction );
	glm::mat4 TransformFromRay( const dd::Ray& ray );

	glm::vec3 DirectionFromPitchYaw( float pitch, float yaw );

	void PitchYawFromDirection( const glm::vec3& dir, float& pitch, float& yaw );

	glm::vec3 NormalFromTriangle( glm::vec3 p0, glm::vec3 p1, glm::vec3 p2 );
}