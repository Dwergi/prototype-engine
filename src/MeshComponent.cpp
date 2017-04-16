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
		Mesh( mesh ),
		Hidden( false ),
		Colour( glm::vec4( 1, 1, 1, 1 ) )
	{

	}
	
	MeshComponent::MeshComponent( const MeshComponent& other ) :
		ComponentBase( other ),
		Mesh( other.Mesh ),
		Hidden( other.Hidden ),
		Colour( other.Colour )
	{

	}

	MeshComponent::~MeshComponent()
	{

	}

	MeshComponent& MeshComponent::operator=( const MeshComponent& other )
	{
		Mesh = other.Mesh;
		Hidden = other.Hidden;
		Colour = other.Colour;

		return *this;
	}
}