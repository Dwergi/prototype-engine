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
		Max( -FLT_MAX, -FLT_MAX, -FLT_MAX )
	{
	}

	AABB::AABB( const AABB& other )
		: Min( other.Min ),
		Max( other.Max )
	{

	}

	AABB::AABB( glm::vec3 min, glm::vec3 max )
		: Min( min ),
		Max( max )
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

	void AABB::Expand( const AABB& bounds )
	{
		Min = glm::min( Min, bounds.Min );
		Max = glm::max( Max, bounds.Max );
	}

	float AABB::Volume() const
	{
		glm::vec3 extents = Extents();
		return extents.x * extents.y * extents.z;
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

	bool AABB::IntersectsRay( const glm::vec3& start, const glm::vec3& dir, float& distance ) const
	{
		glm::vec3 invDir( 1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z );

		float tMin;
		float tMax;

		{
			float xMin = (Min.x - start.x) * invDir.x;
			float xMax = (Max.x - start.x) * invDir.x;

			tMin = dd::min( xMin, xMax );
			tMax = dd::max( xMin, xMax );
		}

		{
			float yMin = (Min.y - start.y) * invDir.y;
			float yMax = (Max.y - start.y) * invDir.y;

			tMin = dd::max( tMin, dd::min( yMin, yMax ) );
			tMax = dd::min( tMax, dd::max( yMin, yMax ) );
		}

		{
			float zMin = (Min.z - start.z) * invDir.z;
			float zMax = (Max.z - start.z) * invDir.z;

			tMin = dd::max( tMin, dd::min( zMin, zMax ) );
			tMax = dd::min( tMax, dd::max( zMin, zMax ) );
		}

		if( tMax >= dd::max( 0.0f, tMin ) )
		{
			distance = tMax;
			return true;
		}

		distance = FLT_MAX;
		return false;
	}

	AABB AABB::GetTransformed( const glm::mat4& transform ) const
	{
		glm::vec3 corners[8];
		GetCorners( corners );

		AABB transformed;
		for( int i = 0; i < 8; ++i )
		{
			glm::vec4 corner = glm::vec4( corners[i], 1.0f ) * transform + transform[3];
			transformed.Expand( corner.xyz() );
		}

		return transformed;
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