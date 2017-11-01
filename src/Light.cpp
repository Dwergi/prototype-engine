#include "PrecompiledHeader.h"
#include "Light.h"

namespace dd
{
	Light::Light()
	{
		m_colour = glm::vec3( 0 );
	}
	
	Light::Light( glm::vec3 colour, float intensity, float ambient, float specular )
	{
		m_colour = colour;
		m_intensity = intensity;
		m_ambient = ambient;
		m_specular = specular;
	}

	Light::Light( const Light& other )
	{
		m_colour = other.m_colour;
		m_intensity = other.m_intensity;
		m_ambient = other.m_ambient;
		m_specular = other.m_specular;
	}

	Light::Light( Light&& other )
	{
		m_colour = other.m_colour;
		m_intensity = other.m_intensity;
		m_ambient = other.m_ambient;
		m_specular = other.m_specular;
	}

	Light::~Light()
	{

	}
}