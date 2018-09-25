//
// MeshComponent.h - A component that specifies that a certain mesh should be drawn at the given location.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "AABB.h"
#include "ComponentType.h"
#include "MeshHandle.h"

namespace dd
{
	struct MeshComponent
	{
	public: 

		ddr::MeshHandle Mesh;

		DD_COMPONENT;

		DD_BEGIN_TYPE( MeshComponent )
			DD_MEMBER( MeshComponent, Mesh )
		DD_END_TYPE
	};
}