//
// TransformComponent.cpp - A component to store the transform of an object.
// Copyright (C) Sebastian Nordgren 
// April 22nd 2017
//

#pragma once

#include "PrecompiledHeader.h"
#include "TransformComponent.h"

#include "EntityManager.h"

namespace dd
{
	TransformComponent::TransformComponent() :
		m_dirty( true )
	{
	}

	TransformComponent::TransformComponent( const TransformComponent& other ) :
		ComponentBase( other ),
		m_local( other.m_local ),
		m_parent( other.m_parent ),
		m_dirty( other.m_dirty )
	{
		if( !m_dirty )
		{
			m_world = other.m_world;
		}
	}

	void TransformComponent::SetLocalPosition( const glm::vec3& pos )
	{
		m_local[3].xyz = pos;
		m_dirty = true;
	}

	void TransformComponent::SetLocalTransform( const glm::mat4& transform )
	{
		m_local = transform;
		m_dirty = true;
	}

	void TransformComponent::UpdateWorldTransform()
	{
		if( m_dirty )
		{
			if( m_parent.IsValid() )
			{
				ComponentHandle<TransformComponent> cmp = m_parent.Get<TransformComponent>();
				cmp.Write()->UpdateWorldTransform();
				m_world = cmp.Write()->GetWorldTransform() * m_local;
			}
			else
			{
				m_world = m_local;
			}

			m_dirty = false;
		}
	}
}