//
// PointLight.h - A point light.
// Copyright (C) Sebastian Nordgren 
// April 21st 2017
//

#pragma once

#include "Mesh.h"

namespace dd
{
	class Camera;

	class PointLight
	{
	public:

		PointLight();
		PointLight( glm::vec3 position, glm::vec3 colour, float intensity );
		PointLight( const PointLight& other );
		~PointLight();

		void SetPosition( glm::vec3 position ) { m_position = position; }
		glm::vec3 GetPosition() const { return m_position; }

		void SetColour( glm::vec3 colour ) { m_colour = colour; }
		glm::vec3 GetColour() const { return m_colour; }

		void SetIntensity( float intensity ) { m_intensity = intensity; }
		const float GetIntensity() const { return m_intensity; }

	private:

		glm::vec3 m_position;
		glm::vec3 m_colour;
		float m_intensity;
	};
}