//
// LightComponent.h - A light.
// Copyright (C) Sebastian Nordgren 
// October 29th 2017
//

#pragma once

#include "ComponentType.h"

namespace ddr
{
	class LightComponent
	{
	public:

		LightComponent();
		LightComponent( const LightComponent& other );
		LightComponent( LightComponent&& other );
		virtual ~LightComponent();

		LightComponent& operator=( const LightComponent& other );
		LightComponent& operator=( LightComponent&& other );

		glm::vec3 Colour;
		float Intensity { 1.0f };
		float Ambient { 0.05f };
		float Specular { 0.5f };
		float Attenuation { 1.0f };
		bool IsDirectional { false };

		DD_COMPONENT;
	};
}