#include "PrecompiledHeader.h"
#include "LightComponent.h"

namespace dd
{
	LightComponent::LightComponent()
	{
		m_colour = glm::vec3( 0 );
	}

	LightComponent::LightComponent( const LightComponent& other )
	{
		m_colour = other.m_colour;
		m_intensity = other.m_intensity;
		m_ambient = other.m_ambient;
		m_specular = other.m_specular;
		m_attenuation = other.m_attenuation;
		m_directional = other.m_directional;
	}

	LightComponent::LightComponent( LightComponent&& other )
	{
		m_colour = other.m_colour;
		m_intensity = other.m_intensity;
		m_ambient = other.m_ambient;
		m_specular = other.m_specular;
		m_attenuation = other.m_attenuation;
		m_directional = other.m_directional;
	}

	LightComponent::~LightComponent()
	{

	}

	LightComponent& LightComponent::operator=( const LightComponent& other )
	{
		m_colour = other.m_colour;
		m_intensity = other.m_intensity;
		m_ambient = other.m_ambient;
		m_specular = other.m_specular;
		m_attenuation = other.m_attenuation;

		return *this;
	}

	LightComponent& LightComponent::operator=( LightComponent&& other )
	{
		m_colour = other.m_colour;
		m_intensity = other.m_intensity;
		m_ambient = other.m_ambient;
		m_specular = other.m_specular;
		m_attenuation = other.m_attenuation;
		m_directional = other.m_directional;

		return *this;
	}
}