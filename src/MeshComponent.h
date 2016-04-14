//
// MeshComponent.h - A component that specifies that a certain mesh should be drawn at the given location.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "Component.h"
#include "DenseMapPool.h"

namespace dd
{
	class Mesh;

	class MeshComponent : public Component
	{
	public: 

		typedef DenseMapPool<MeshComponent> Pool;

		MeshComponent();
		MeshComponent( Mesh& mesh );
		MeshComponent( const MeshComponent& other );
		~MeshComponent();

		void SetMesh( Mesh* mesh );
		Mesh* GetMesh() const { return m_mesh; }

		BASIC_TYPE( MeshComponent )

	private:
		Mesh* m_mesh;
	};
}