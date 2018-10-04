//
// Ray.cpp - A ray wrapper class.
// Copyright (C) Sebastian Nordgren 
// April 24th 2017
//

#include "PCH.h"
#include "Ray.h"

DD_TYPE_CPP( dd::Ray );

namespace dd
{
	Ray::Ray()
	{
		SetDirection( glm::vec3( 0, 0, 1 ) );
	}
	
	Ray::Ray( const Ray& other ) :
		m_origin( other.m_origin ),
		m_direction( other.m_direction ),
		m_invDir( other.m_invDir ),
		m_length( other.m_length )
	{
	}

	Ray::Ray( const glm::vec3& origin, const glm::vec3& direction, float length ) :
		m_origin( origin ),
		m_length( length )
	{
		SetDirection( direction );
	}

	void Ray::operator=( const Ray& other )
	{
		m_origin = other.m_origin;
		m_direction = other.m_direction;
		m_invDir = other.m_invDir;
		m_length = other.m_length;
	}

	void Ray::SetDirection( const glm::vec3& direction )
	{
		m_direction = glm::normalize( direction );
		m_invDir = glm::vec3( 1.0f ) / m_direction;
	}
}
