//
// HitState.cpp - State information for a hit test.
// Copyright (C) Sebastian Nordgren 
// September 17th 2017
//

#include "PrecompiledHeader.h"
#include "HitState.h"

namespace dd
{
	HitState::HitState()
	{
	}

	HitState::HitState( dd::Ray ray, float length ) :
		m_status( Status::Pending ),
		m_ray( ray ),
		m_length( length )
	{
	}

	HitState::HitState( const HitState& other ) :
		m_entity( other.m_entity ),
		m_status( other.m_status ),
		m_ray( other.m_ray ),
		m_length( other.m_length ),
		m_distance( other.m_distance )
	{
	}

	void HitState::operator=( const HitState& other )
	{
		m_entity = other.m_entity;
		m_status = other.m_status;
		m_ray = other.m_ray;
		m_length = other.m_length;
		m_distance = other.m_distance;
	}

	void HitState::RegisterHit( float distance, ddc::Entity entity )
	{
		DD_ASSERT( IsValid() && !IsCompleted() );

		if( distance < m_length && distance < m_distance )
		{
			m_distance = distance;
			m_entity = entity;
		}
	}

	void HitState::SetCompleted()
	{
		DD_ASSERT( IsValid() && !IsCompleted() );
		m_status = Status::Completed;
	}
}