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
	class MeshComponent
	{
	public: 

		MeshComponent();
		MeshComponent( ddr::MeshHandle mesh );
		MeshComponent( const MeshComponent& other );
		~MeshComponent();

		ddr::MeshHandle Mesh;
		glm::vec4 Colour;

		DD_COMPONENT;

		DD_BEGIN_TYPE( MeshComponent )
			DD_MEMBER( MeshComponent, Mesh )
			DD_MEMBER( MeshComponent, Colour )
		DD_END_TYPE
	};
}