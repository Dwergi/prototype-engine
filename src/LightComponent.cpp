//
// LightComponent.cpp - A light.
// Copyright (C) Sebastian Nordgren 
// October 29th 2017
//

#include "PCH.h"
#include "LightComponent.h"

DD_TYPE_CPP( dd::LightComponent );

DD_ENUM_CPP( dd::LightType )
{
	DD_ENUM_OPTION( dd::LightType, Directional );
	DD_ENUM_OPTION( dd::LightType, Point );
	DD_ENUM_OPTION( dd::LightType, Spot );
};
