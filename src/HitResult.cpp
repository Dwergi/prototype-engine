//
// HitResult.cpp - State information for a hit test.
// Copyright (C) Sebastian Nordgren 
// September 17th 2017
//

#include "PrecompiledHeader.h"
#include "HitResult.h"

namespace dd
{
	HitResult::HitResult()
	{
	}

	HitResult::HitResult( dd::Ray ray ) :
		m_ray( ray )
	{
	}

	HitResult::HitResult( dd::Ray ray, float length ) :
		m_ray( ray ),
		m_length( length )
	{
	}

	HitResult::HitResult( const HitResult& other ) :
		m_entity( other.m_entity ),
		m_ray( other.m_ray ),
		m_length( other.m_length ),
		m_distance( other.m_distance )
	{
	}

	void HitResult::operator=( const HitResult& other )
	{
		m_entity = other.m_entity;
		m_ray = other.m_ray;
		m_length = other.m_length;
		m_distance = other.m_distance;
	}

	void HitResult::RegisterHit( float distance, ddc::Entity entity )
	{
		if( distance < m_length && distance < m_distance )
		{
			m_distance = distance;
			m_entity = entity;
		}
	}
}