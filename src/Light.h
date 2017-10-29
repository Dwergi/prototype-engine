//
// Light.h - A light.
// Copyright (C) Sebastian Nordgren 
// October 29th 2017
//

#pragma once

namespace dd
{
	class Light
	{
	public:

		virtual ~Light();

		void SetColour( glm::vec3 colour ) { m_colour = colour; }
		glm::vec3 GetColour() const { return m_colour; }

		void SetIntensity( float intensity ) { m_intensity = intensity; }
		const float GetIntensity() const { return m_intensity; }

	protected:
		Light();
		Light( glm::vec3 colour, float intensity );

		glm::vec3 m_colour;
		float m_intensity;
	};
}