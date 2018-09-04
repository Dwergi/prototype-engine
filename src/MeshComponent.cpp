//
// MeshComponent.cpp - A component that specifies that a certain mesh should be drawn at the given location.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#include "PrecompiledHeader.h"
#include "MeshComponent.h"

#include "Mesh.h"

DD_COMPONENT_CPP( dd::MeshComponent );

namespace dd
{
	MeshComponent::MeshComponent() : 
		Colour( glm::vec4( 1, 1, 1, 1 ) )
	{
	}
	
	MeshComponent::MeshComponent( ddr::MeshHandle mesh ) :
		Mesh( mesh ),
		Colour( glm::vec4( 1, 1, 1, 1 ) )
	{
	}
	
	MeshComponent::MeshComponent( const MeshComponent& other ) :
		Mesh( other.Mesh ),
		Colour( other.Colour )
	{
	}

	MeshComponent::~MeshComponent()
	{

	}
}