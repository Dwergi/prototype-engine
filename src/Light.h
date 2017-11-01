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
		float GetIntensity() const { return m_intensity; }

		void SetAmbient( float ambient ) { m_ambient = ambient; }
		float GetAmbient() const { return m_ambient; }

		void SetSpecular( float specular ) { m_specular = specular; }
		float GetSpecular() const { return m_specular; }

	protected:
		Light();
		Light( glm::vec3 colour, float intensity, float ambient, float specular );
		Light( const Light& other );
		Light( Light&& other );

		glm::vec3 m_colour;
		float m_intensity { 1.0f };
		float m_ambient { 0.05f };
		float m_specular { 0.5f };
	};
}