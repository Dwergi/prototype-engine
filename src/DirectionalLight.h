//
// DirectionalLight.h - A directional light.
// Copyright (C) Sebastian Nordgren 
// October 29th 2017
//

#pragma once

#include "Light.h"

namespace dd
{
	class DirectionalLight : public Light
	{
	public:

		DirectionalLight();
		DirectionalLight( const DirectionalLight& other );
		DirectionalLight( DirectionalLight&& other );
		DirectionalLight( glm::vec3 direction, glm::vec3 colour, float intensity, float ambient = 0.05f, float specular = 0.5f );
		virtual ~DirectionalLight();

		glm::vec3 GetDirection() const { return m_direction; }
		void SetDirection( glm::vec3 direction ) { m_direction = glm::normalize( direction ); }

	private:

		glm::vec3 m_direction;
	};
}