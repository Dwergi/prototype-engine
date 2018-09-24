//
// Plane.cpp - A simple plane equation with some supporting functions.
// Copyright (C) Sebastian Nordgren 
// November 4th 2015
//

#include "PrecompiledHeader.h"
#include "Plane.h"

#include <cmath>

namespace dd
{
	Plane::Plane()
	{

	}

	Plane::Plane( const glm::vec4& params )
		: Plane( params.x, params.y, params.z, params.w )
	{

	}

	Plane::Plane( const glm::vec3& point, const glm::vec3& normal )
	{
		m_plane.xyz = glm::normalize( normal );
		m_plane.w = -glm::dot( normal, point );
	}

	Plane::Plane( float a, float b, float c, float d )
	{
		glm::vec3 normal( a, b, c );

		float len = glm::length( normal );
		normal /= len;

		m_plane.xyz = normal;
		m_plane.w = d / len;
	}

	Plane::Plane( const glm::vec3& pt1, const glm::vec3& pt2, const glm::vec3& pt3 )
	{
		glm::vec3 diff1 = pt1 - pt2;
		glm::vec3 diff2 = pt3 - pt2;

		glm::vec3 normal = glm::normalize( glm::cross( diff2, diff1 ) );
		
		m_plane.xyz = normal;
		m_plane.w = -glm::dot( normal, pt2 );
	}

	Plane::Plane( const Plane& other )
		: m_plane( other.m_plane )
	{

	}

	Plane::~Plane()
	{

	}

	float Plane::DistanceTo( const glm::vec3& point ) const
	{
		return glm::dot( m_plane.xyz(), point ) + m_plane.w;
	}

	Plane Plane::GetTransformed( const glm::mat4& transform ) const
	{
		glm::vec3 origin = (transform * glm::vec4( Origin(), 1 )).xyz;
		glm::vec3 normal = glm::normalize( glm::vec3( (transform * glm::vec4( Normal(), 0 )).xyz ) );
		return Plane( origin, normal );
	}
}