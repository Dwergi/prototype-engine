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

	DD_ENUM( dd::LightType );

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

		DD_CLASS( dd::LightComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::LightComponent, Colour );
			DD_MEMBER( dd::LightComponent, Intensity );
			DD_MEMBER( dd::LightComponent, Ambient );
			DD_MEMBER( dd::LightComponent, Specular );
			DD_MEMBER( dd::LightComponent, Attenuation );
			DD_MEMBER( dd::LightComponent, InnerAngle );
			DD_MEMBER( dd::LightComponent, OuterAngle );
			DD_MEMBER( dd::LightComponent, LightType );
		}
	};
}