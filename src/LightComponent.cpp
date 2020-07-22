//
// LightComponent.cpp - A light.
// Copyright (C) Sebastian Nordgren 
// October 29th 2017
//

#include "PCH.h"
#include "LightComponent.h"

DD_COMPONENT_CPP(dd::LightComponent);

DD_BEGIN_ENUM(dd::LightType)
	DD_ENUM_OPTION(Directional);
	DD_ENUM_OPTION(Point);
	DD_ENUM_OPTION(Spot);
DD_END_ENUM()