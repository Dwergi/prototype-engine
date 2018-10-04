//
// HitResult.cpp - State information for a hit test.
// Copyright (C) Sebastian Nordgren 
// September 17th 2017
//

#include "PCH.h"
#include "HitResult.h"

DD_TYPE_CPP( dd::HitHandle );

namespace dd
{
	HitResult::HitResult()
	{
	}

	HitResult::HitResult( dd::Ray ray ) :
		m_ray( ray )
	{
	}

	HitResult::HitResult( const HitResult& other ) :
		m_entity( other.m_entity ),
		m_ray( other.m_ray ),
		m_distance( other.m_distance ),
		m_normal( other.m_normal )
	{
	}

	void HitResult::operator=( const HitResult& other )
	{
		m_entity = other.m_entity;
		m_ray = other.m_ray;
		m_distance = other.m_distance;
		m_normal = other.m_normal;
	}

	void HitResult::RegisterHit( float distance, glm::vec3 normal, ddc::Entity entity )
	{
		if( distance < m_ray.Length() && distance < m_distance )
		{
			m_distance = distance;
			m_normal = normal;
			m_entity = entity;
		}
	}
}