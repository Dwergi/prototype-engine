//
// ParticleSystemComponent.cpp - A particle system component.
// Copyright (C) Sebastian Nordgren 
// August 12th 2018
//

#include "PrecompiledHeader.h"
#include "ParticleSystemComponent.h"

namespace ddc
{
	DD_COMPONENT_CPP( ParticleSystemComponent );

	ParticleSystemComponent::ParticleSystemComponent() :
		m_minLifetime( 0.5 ), m_maxLifetime( 5.0 ),
		m_minVelocity( -10.0f, -10.0f, -10.0f ), m_maxVelocity( 10.0f, 10.0f, 10.0f ),
		m_minColour( 0, 0, 0 ), m_maxColour( 1, 1, 1 ),
		m_minSize( 0, 0 ), m_maxSize( 1, 1 ),
		m_rng( m_seed )
	{
	}
}