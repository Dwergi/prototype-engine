//
// AABB.cpp - An axis-aligned bounding box.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PrecompiledHeader.h"
#include "AABB.h"

namespace dd
{
	AABB::AABB()
		: Min( FLT_MAX, FLT_MAX, FLT_MAX ),
		Max( FLT_MIN, FLT_MIN, FLT_MIN )
	{

	}

	AABB::AABB( const AABB& other )
		: Min( other.Min ),
		Max( other.Max )
	{

	}

	AABB::~AABB()
	{

	}

	void AABB::Expand( const glm::vec3& pt )
	{
		Min = glm::min( Min, pt );
		Max = glm::max( Max, pt );
	}

	bool AABB::Contains( const glm::vec3& pt ) const
	{
		return glm::all( glm::lessThanEqual( Min, pt ) ) && glm::all( glm::greaterThanEqual( Max, pt ) );
	}

	bool AABB::Contains( const AABB& other ) const
	{
		return Contains( other.Min ) && Contains( other.Max );
	}

	void AABB::GetCorners( glm::vec3 (&corners)[8] ) const
	{
		for( int i = 0; i < 8; ++i )
		{
			corners[i] = Min;

			if( i & 0x4 )
				corners[i].x = Max.x;

			if( i & 0x2 )
				corners[i].y = Max.y;

			if( i & 0x1 )
				corners[i].z = Max.z;
		}
	}

	bool AABB::Intersects( const AABB& other ) const
	{
		return glm::all( glm::lessThan( Min, other.Max ) ) && glm::all( glm::greaterThan( Max, other.Min ) );
	}

	glm::vec3 AABB::Center() const
	{
		return (Min + Max) * 0.5f;
	}

	glm::vec3 AABB::Extents() const
	{
		return Max - Min;
	}

	bool AABB::operator==( const AABB& other ) const
	{
		return Min == other.Min && Max == other.Max;
	}

	bool AABB::operator!=( const AABB& other ) const
	{
		return !operator==( other );
	}
}