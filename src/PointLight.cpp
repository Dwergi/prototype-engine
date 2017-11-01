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
	PointLight::PointLight()
	{
		m_position = glm::vec3( 0 );
		m_attenuation = 0;
	}

	PointLight::PointLight( glm::vec3 position, glm::vec3 colour, float intensity, float attenuation, float ambient, float specular ) :
		Light( colour, intensity, ambient, specular ),
		m_position( position ),
		m_attenuation( attenuation )
	{
	}
	
	PointLight::PointLight( const PointLight& other ) :
		Light( other ),
		m_position( other.m_position ),
		m_attenuation( other.m_attenuation )
	{
	}

	PointLight::PointLight( PointLight&& other ) :
		Light( other ),
		m_position( other.m_position ),
		m_attenuation( other.m_attenuation )
	{
	}

	PointLight::~PointLight()
	{

	}
}