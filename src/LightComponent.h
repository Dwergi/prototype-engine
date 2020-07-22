//
// LightComponent.h - A light.
// Copyright (C) Sebastian Nordgren 
// October 29th 2017
//

#pragma once

namespace dd
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

		DD_BEGIN_CLASS(dd::LightComponent)
			DD_COMPONENT();

			DD_MEMBER(Colour);
			DD_MEMBER(Intensity);
			DD_MEMBER(Ambient);
			DD_MEMBER(Specular);
			DD_MEMBER(Attenuation);
			DD_MEMBER(InnerAngle);
			DD_MEMBER(OuterAngle);
			DD_MEMBER(LightType);
		DD_END_CLASS()
	};
}

DD_ENUM(dd::LightType)
