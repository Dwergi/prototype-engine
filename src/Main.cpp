//
// Main.cpp - Entry point into application.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#include "PrecompiledHeader.h"

#ifdef _TEST
	#include "Tests.h"
#else
	#include "EntitySystem.h"
	#include "Services.h"
	#include "TransformComponent.h"
	#include "ScopedTimer.h"
	#include "OctreeComponent.h"
	#include "Recorder.h"
	#include "PropertyList.h"
#endif
//---------------------------------------------------------------------------

#ifndef _TEST
	Services g_services;
#endif

int main( int argc, char* const argv[] )
{
#ifdef _TEST
	int iError = tests::RunTests( argc, argv );

	if( iError != 0 )
		ASSERT( false, "Tests failed!" );

	return iError;
#else
	EntitySystem entitySystem;
	g_services.Register<EntitySystem>( &entitySystem );

	TransformComponent::Pool transform_pool;
	g_services.Register( &transform_pool );

	OctreeComponent::Pool octree_pool;
	g_services.Register( &octree_pool );



	ASSERT( false, "DONE!" );
	return 0;
#endif
}
//===========================================================================
