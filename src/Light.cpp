#include "PrecompiledHeader.h"
#include "Light.h"

namespace dd
{
	Light::Light()
	{
		m_colour = glm::vec3( 0 );
		m_intensity = 0;
	}
	
	Light::Light( glm::vec3 colour, float intensity )
	{
		m_colour = colour;
		m_intensity = intensity;
	}

	Light::~Light()
	{

	}
}