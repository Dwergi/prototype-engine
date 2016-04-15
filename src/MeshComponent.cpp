//
// MeshComponent.cpp - A component that specifies that a certain mesh should be drawn at the given location.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#include "PrecompiledHeader.h"
#include "MeshComponent.h"

#include "Mesh.h"

namespace dd
{
	MeshComponent::MeshComponent()
	{

	}
	
	MeshComponent::MeshComponent( MeshHandle mesh ) :
		m_mesh( mesh )
	{

	}
	
	MeshComponent::MeshComponent( const MeshComponent& other ) :
		Component( other ),
		m_mesh( other.m_mesh )
	{

	}

	MeshComponent::~MeshComponent()
	{

	}

	void MeshComponent::SetMesh( MeshHandle mesh )
	{
		m_mesh = mesh;
	}
}