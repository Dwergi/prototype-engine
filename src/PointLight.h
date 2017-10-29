//
// PointLight.h - A point light.
// Copyright (C) Sebastian Nordgren 
// April 21st 2017
//

#pragma once

#include "Light.h"

namespace dd
{
	class PointLight : public Light
	{
	public:

		PointLight();
		PointLight( glm::vec3 position, float attenuation, glm::vec3 colour, float intensity );
		PointLight( const PointLight& other );
		virtual ~PointLight();

		void SetPosition( glm::vec3 position ) { m_position = position; }
		glm::vec3 GetPosition() const { return m_position; }

		void SetAttenuation( float attenuation ) { m_attenuation = attenuation; }
		float GetAttenuation() const { return m_attenuation; }

	private:

		glm::vec3 m_position;
		float m_attenuation;
	};
}