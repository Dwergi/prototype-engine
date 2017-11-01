//
// DirectionalLight.cpp - A directional light.
// Copyright (C) Sebastian Nordgren 
// October 29th 2017
//

#include "PrecompiledHeader.h"
#include "DirectionalLight.h"

namespace dd
{
	DirectionalLight::DirectionalLight()
	{
		m_direction = glm::vec3( 0 );
	}

	DirectionalLight::DirectionalLight( const DirectionalLight& other )
		: Light( other ),
		m_direction( other.m_direction )
	{

	}

	DirectionalLight::DirectionalLight( DirectionalLight&& other )
		: Light( other ),
		m_direction( other.m_direction )
	{

	}

	DirectionalLight::DirectionalLight( glm::vec3 direction, glm::vec3 colour, float intensity, float ambient, float specular )
		: Light( colour, intensity, ambient, specular )
	{
		m_direction = direction;
	}

	DirectionalLight::~DirectionalLight()
	{

	}
}