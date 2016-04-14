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
	MeshComponent::MeshComponent() :
		m_mesh( nullptr )
	{

	}
	
	MeshComponent::MeshComponent( Mesh& mesh ) :
		m_mesh( &mesh )
	{
		m_mesh->AddRef();
	}
	
	MeshComponent::MeshComponent( const MeshComponent& other ) :
		Component( other ),
		m_mesh( other.m_mesh )
	{
		if( m_mesh != nullptr )
		{
			m_mesh->AddRef();
		}
	}

	MeshComponent::~MeshComponent()
	{
		if( m_mesh != nullptr )
		{
			m_mesh->RemoveRef();
		}
	}

	void MeshComponent::SetMesh( Mesh* mesh )
	{
		if( m_mesh != nullptr )
		{
			m_mesh->RemoveRef();
		}

		m_mesh = mesh;

		if( m_mesh != nullptr )
		{
			m_mesh->AddRef();
		}
	}
}