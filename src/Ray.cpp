//
// Ray.cpp - A ray wrapper class.
// Copyright (C) Sebastian Nordgren 
// April 24th 2017
//

#include "PCH.h"
#include "Ray.h"

DD_TYPE_CPP( ddm::Ray );

namespace ddm
{
	Ray::Ray()
	{
		SetDirection( glm::vec3( 0, 0, 1 ) );
	}
	
	Ray::Ray( const ddm::Ray& other ) :
		m_origin( other.m_origin ),
		m_direction( other.m_direction ),
		m_invDir( other.m_invDir ),
		Length( other.Length )
	{
	}

	Ray::Ray( const glm::vec3& origin, const glm::vec3& direction, float length ) :
		m_origin( origin ),
		Length( length )
	{
		SetDirection( direction );
	}

	void Ray::operator=( const ddm::Ray& other )
	{
		m_origin = other.m_origin;
		m_direction = other.m_direction;
		m_invDir = other.m_invDir;
		Length = other.Length;
	}

	void Ray::SetDirection( const glm::vec3& direction )
	{
		m_direction = glm::normalize( direction );
		m_invDir = glm::vec3( 1.0f ) / m_direction;
	}
}
