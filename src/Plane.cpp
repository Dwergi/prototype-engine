//
// Plane.cpp - A simple plane equation with some supporting functions.
// Copyright (C) Sebastian Nordgren 
// November 4th 2015
//

#include "PrecompiledHeader.h"
#include "Plane.h"

namespace dd
{
	Plane::Plane( const glm::vec3& point, const glm::vec3& normal )
	{
		m_plane.xyz = normal;
		m_plane.w = -glm::dot( normal, point );
	}

	Plane::Plane( const Plane& other )
		: m_plane( other.m_plane )
	{

	}

	Plane::~Plane()
	{

	}

	glm::vec3 Plane::Normal() const
	{
		return glm::vec3( m_plane.x, m_plane.y, m_plane.z );
	}

	glm::vec3 Plane::Origin() const
	{
		return Normal() -m_plane.w;
	}

	float Plane::DistanceTo( const glm::vec3& point ) const
	{
		return float( m_plane.x * point.x + m_plane.y * point.y + m_plane.z * point.z + m_plane.w );
	}
}