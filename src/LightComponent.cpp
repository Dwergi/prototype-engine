#include "PrecompiledHeader.h"
#include "LightComponent.h"

DD_COMPONENT_CPP( dd::LightComponent );

namespace dd
{
	LightComponent::LightComponent()
	{
		Colour = glm::vec3( 0 );
	}

	LightComponent::LightComponent( const LightComponent& other )
	{
		Colour = other.Colour;
		Intensity = other.Intensity;
		Ambient = other.Ambient;
		Specular = other.Specular;
		Attenuation = other.Attenuation;
		IsDirectional = other.IsDirectional;
	}

	LightComponent::LightComponent( LightComponent&& other )
	{
		Colour = other.Colour;
		Intensity = other.Intensity;
		Ambient = other.Ambient;
		Specular = other.Specular;
		Attenuation = other.Attenuation;
		IsDirectional = other.IsDirectional;
	}

	LightComponent::~LightComponent()
	{

	}

	LightComponent& LightComponent::operator=( const LightComponent& other )
	{
		Colour = other.Colour;
		Intensity = other.Intensity;
		Ambient = other.Ambient;
		Specular = other.Specular;
		Attenuation = other.Attenuation;
		IsDirectional = other.IsDirectional;

		return *this;
	}

	LightComponent& LightComponent::operator=( LightComponent&& other )
	{
		Colour = other.Colour;
		Intensity = other.Intensity;
		Ambient = other.Ambient;
		Specular = other.Specular;
		Attenuation = other.Attenuation;
		IsDirectional = other.IsDirectional;

		return *this;
	}
}