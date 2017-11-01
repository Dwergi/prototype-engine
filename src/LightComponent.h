//
// LightComponent.h - A light.
// Copyright (C) Sebastian Nordgren 
// October 29th 2017
//

#pragma once

#include "ComponentBase.h"
#include "DenseMapPool.h"

namespace dd
{
	class LightComponent : public ComponentBase
	{
	public:

		using Pool = DenseMapPool<LightComponent>;

		LightComponent();
		LightComponent( const LightComponent& other );
		LightComponent( LightComponent&& other );
		virtual ~LightComponent();

		LightComponent& operator=( const LightComponent& other );
		LightComponent& operator=( LightComponent&& other );

		void SetColour( glm::vec3 colour ) { m_colour = colour; }
		glm::vec3 GetColour() const { return m_colour; }

		void SetDirectional( bool directional ) { m_directional = directional; }
		bool IsDirectional() const { return m_directional; }

		void SetIntensity( float intensity ) { m_intensity = intensity; }
		float GetIntensity() const { return m_intensity; }

		void SetAmbient( float ambient ) { m_ambient = ambient; }
		float GetAmbient() const { return m_ambient; }

		void SetSpecular( float specular ) { m_specular = specular; }
		float GetSpecular() const { return m_specular; }

		void SetAttenuation( float attenuation ) { m_attenuation = attenuation; }
		float GetAttenuation() const { return m_attenuation; }

	protected:

		glm::vec3 m_colour;
		float m_intensity { 1.0f };
		float m_ambient { 0.05f };
		float m_specular { 0.5f };
		float m_attenuation { 1.0f };
		bool m_directional { false };
	};
}