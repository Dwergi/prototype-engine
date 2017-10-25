//
// MeshComponent.h - A component that specifies that a certain mesh should be drawn at the given location.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "ComponentBase.h"
#include "DenseMapPool.h"
#include "Mesh.h"
#include "AABB.h"

namespace dd
{
	class TransformComponent;

	class MeshComponent : public ComponentBase
	{
	public: 

		using Pool = DenseMapPool<MeshComponent>;

		MeshComponent();
		MeshComponent( MeshHandle mesh );
		MeshComponent( const MeshComponent& other );
		~MeshComponent();

		MeshHandle Mesh;
		bool Hidden;
		glm::vec4 Colour;
		AABB Bounds;

		MeshComponent& operator=( const MeshComponent& other );
		void UpdateBounds( const glm::mat4& transform );

		BEGIN_TYPE( MeshComponent )
			MEMBER( MeshComponent, Mesh )
			MEMBER( MeshComponent, Hidden )
			MEMBER( MeshComponent, Colour )
		END_TYPE
	};
}