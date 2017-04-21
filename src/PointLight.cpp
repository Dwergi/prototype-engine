//
// PointLight.h - A point light.
// Copyright (C) Sebastian Nordgren 
// April 21st 2017
//

#include "PrecompiledHeader.h"
#include "PointLight.h"

#include "glm/gtx/transform.hpp"

namespace dd
{
	PointLight::PointLight() :
		m_intensity( 1.0f )
	{

	}

	PointLight::PointLight( glm::vec3 position, glm::vec3 colour, float intensity ) :
		m_position( position ),
		m_colour( colour ),
		m_intensity( intensity )
	{
		
	}
	
	PointLight::PointLight( const PointLight& other ) :
		m_position( other.m_position ),
		m_colour( other.m_colour ),
		m_intensity( other.m_intensity )
	{

	}

	PointLight::~PointLight()
	{

	}
}