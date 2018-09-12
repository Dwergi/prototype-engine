//
// Ray.cpp - A ray wrapper class.
// Copyright (C) Sebastian Nordgren 
// April 24th 2017
//

#include "PrecompiledHeader.h"
#include "Ray.h"

namespace dd
{
	Ray::Ray()
	{
		SetDirection( glm::vec3( 0, 0, 1 ) );
	}
	
	Ray::Ray( const Ray& other ) :
		m_origin( other.m_origin ),
		m_direction( other.m_direction ),
		m_invDir( other.m_invDir )
	{
	}

	Ray::Ray( const glm::vec3& origin, const glm::vec3& direction ) :
		m_origin( origin )
	{
		SetDirection( direction );
	}

	void Ray::operator=( const Ray& other )
	{
		m_origin = other.m_origin;
		m_direction = other.m_direction;
		m_invDir = other.m_invDir;
	}

	void Ray::SetDirection( const glm::vec3& direction )
	{
		m_direction = glm::normalize( direction );
		m_invDir = glm::vec3( 1.0f ) / m_direction;
	}
}
