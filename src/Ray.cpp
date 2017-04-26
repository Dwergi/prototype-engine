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

	}

	Ray::Ray( const glm::vec3& origin, const glm::vec3& direction ) :
		m_origin( origin ),
		m_direction( direction )
	{

	}

	void Ray::SetDirection( const glm::vec3& direction )
	{
		m_direction = glm::normalize( direction );
	}
}
