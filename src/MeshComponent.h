//
// MeshComponent.h - A component that specifies that a certain mesh should be drawn at the given location.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "ComponentBase.h"
#include "DenseMapPool.h"
#include "Mesh.h"

namespace dd
{
	class MeshComponent : public ComponentBase
	{
	public: 

		typedef DenseMapPool<MeshComponent> Pool;

		MeshComponent();
		MeshComponent( MeshHandle mesh );
		MeshComponent( const MeshComponent& other );
		~MeshComponent();

		MeshHandle Mesh;
		bool Hidden;
		glm::vec4 Colour;

		BEGIN_TYPE( MeshComponent )
			MEMBER( MeshComponent, Mesh )
			MEMBER( MeshComponent, Hidden )
			MEMBER( MeshComponent, Colour )
		END_TYPE
	};
}