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
		bool Hidden;
		glm::vec4 Colour;
		AABB Bounds;

		DD_COMPONENT;

		BEGIN_TYPE( MeshComponent )
			MEMBER( MeshComponent, Mesh )
			MEMBER( MeshComponent, Hidden )
			MEMBER( MeshComponent, Colour )
		END_TYPE
	};
}