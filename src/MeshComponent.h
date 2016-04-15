//
// MeshComponent.h - A component that specifies that a certain mesh should be drawn at the given location.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "Component.h"
#include "DenseMapPool.h"
#include "Mesh.h"

namespace dd
{
	class MeshComponent : public Component
	{
	public: 

		typedef DenseMapPool<MeshComponent> Pool;

		MeshComponent();
		MeshComponent( MeshHandle mesh );
		MeshComponent( const MeshComponent& other );
		~MeshComponent();

		void SetMesh( MeshHandle mesh );
		MeshHandle GetMesh() const { return m_mesh; }

		BASIC_TYPE( MeshComponent )

	private:
		MeshHandle m_mesh;
	};
}