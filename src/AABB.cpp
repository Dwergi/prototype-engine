//
// AABB.cpp - An axis-aligned bounding box.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PrecompiledHeader.h"
#include "AABB.h"

#include "Ray.h"

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

	void AABB::Clear()
	{
		Min = glm::vec3( FLT_MAX, FLT_MAX, FLT_MAX );
		Max = glm::vec3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	}

	bool AABB::IsValid() const
	{
		return Min != glm::vec3( FLT_MAX, FLT_MAX, FLT_MAX ) || Max != glm::vec3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
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
		return glm::all( glm::lessThanEqual( Min, other.Max ) ) && 
			glm::all( glm::greaterThanEqual( Max, other.Min ) );
	}

	bool AABB::IntersectsRay( const glm::vec3& start, const glm::vec3& dir, float& distance ) const
	{
		Ray ray( start, dir );

		return IntersectsRay( ray, distance );
	}

	bool AABB::IntersectsRay( const Ray& ray ) const
	{
		float ignored;
		return IntersectsRay( ray, ignored );
	}

	bool AABB::IntersectsRay( const Ray& ray, float& distance ) const
	{
		glm::vec3 vMin = (Min - ray.Origin()) * ray.InverseDir();
		glm::vec3 vMax = (Max - ray.Origin()) * ray.InverseDir();

		float tMin = 0.0f;
		float tMax = INFINITE;

		if( ray.Direction().x != 0.0f )
		{
			tMin = dd::min( vMin.x, vMax.x );
			tMax = dd::max( vMin.x, vMax.x );
		}

		if( ray.Direction().y != 0.0f )
		{
			tMin = dd::max( tMin, dd::min( vMin.y, vMax.y ) );
			tMax = dd::min( tMax, dd::max( vMin.y, vMax.y ) );
		}

		if( ray.Direction().z != 0.0f )
		{
			tMin = dd::max( tMin, dd::min( vMin.z, vMax.z ) );
			tMax = dd::min( tMax, dd::max( vMin.z, vMax.z ) );
		}

		if( tMax > max( tMin, 0.0f ) )
		{
			distance = tMin;
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
			glm::vec4 corner = transform * glm::vec4( corners[i], 1.0f );
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