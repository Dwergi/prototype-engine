//
// ParticleSystemComponent.cpp - A particle system component.
// Copyright (C) Sebastian Nordgren 
// August 12th 2018
//

#include "PCH.h"
#include "ParticleSystemComponent.h"

DD_TYPE_CPP( dd::ParticleSystemComponent );

namespace dd
{
	ParticleSystemComponent::ParticleSystemComponent() :
		MinLifetime( 0.5 ), MaxLifetime( 5.0 ),
		MinVelocity( -10.0f, -10.0f, -10.0f ), MaxVelocity( 10.0f, 10.0f, 10.0f ),
		MinColour( 0, 0, 0 ), MaxColour( 1, 1, 1 ),
		MinSize( 0, 0 ), MaxSize( 1, 1 ),
		RNG( Seed )
	{
	}
}