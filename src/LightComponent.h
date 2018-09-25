//
// LightComponent.h - A light.
// Copyright (C) Sebastian Nordgren 
// October 29th 2017
//

#pragma once

#include "ComponentType.h"

namespace ddr
{
	enum class LightType
	{
		Directional,
		Point,
		Spot
	};

	class LightComponent
	{
	public:

		glm::vec3 Colour;
		float Intensity { 5.0f };
		float Ambient { 0.05f };
		float Specular { 0.5f };
		float Attenuation { 0.2f };

		float InnerAngle { 0.0f };
		float OuterAngle { 0.0f };

		LightType LightType { LightType::Directional };

		DD_COMPONENT;
	};
}