//
// FluxBulletComponent.cpp
// Copyright (C) Sebastian Nordgren 
// July 22nd 2020
//

#include "PCH.h"
#include "FluxBulletComponent.h"

DD_COMPONENT_CPP(flux::FluxBulletComponent);

DD_BEGIN_ENUM(flux::BulletType)
	DD_ENUM_OPTION(Friendly);
	DD_ENUM_OPTION(Enemy);
DD_END_ENUM()