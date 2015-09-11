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
#include "DoubleBuffer.h"
#include "SwarmAgentComponent.h"
#include "SwarmSystem.h"
#include "Window.h"
#include "Input.h"

#endif
//---------------------------------------------------------------------------

#ifndef _TEST

template<typename T>
void RegisterComponent()
{
	dd::DoubleBuffer<typename T::Pool>* double_buffer = new dd::DoubleBuffer<typename T::Pool>( new typename T::Pool(), new typename T::Pool() );
	g_services.Register( double_buffer );
}

template<typename T>
typename T::Pool* GetWritePool()
{
	return g_services.Get<dd::DoubleBuffer<typename T::Pool>>().GetWrite();
}

template<typename T>
typename T::Pool* GetReadPool()
{
	return g_services.Get<dd::DoubleBuffer<typename T::Pool>>().GetRead();
}

template<typename T>
dd::DoubleBuffer<typename T::Pool>& GetDoubleBuffer()
{
	dd::DoubleBuffer<typename T::Pool>& buffer = g_services.Get<dd::DoubleBuffer<typename T::Pool>>();

	return buffer;
}

	dd::Services g_services;
#endif

int main( int argc, char* const argv[] )
{
	for( int i = 1; i < argc; ++i )
	{
		if( strcmp( argv[ i ], "-noassert" ) == 0 )
		{
			pempek::assert::implementation::ignoreAllAsserts( true );
		}
	}

#ifdef _TEST
	int iError = tests::RunTests( argc, argv );

	if( iError != 0 )
		ASSERT( false, "Tests failed!" );

	return iError;
#else
	dd::EntitySystem entitySystem;
	g_services.Register<dd::EntitySystem>( &entitySystem );

	RegisterComponent<dd::TransformComponent>();
	RegisterComponent<dd::OctreeComponent>();
	RegisterComponent<dd::SwarmAgentComponent>();

	auto& transform_db = GetDoubleBuffer<dd::TransformComponent>();
	auto& swarm_db = GetDoubleBuffer<dd::SwarmAgentComponent>();
	auto& octree_db = GetDoubleBuffer<dd::OctreeComponent>();

	auto& transform_pool = transform_db.GetWrite();
	auto& swarm_pool = swarm_db.GetWrite();
	auto& octree_pool = octree_db.GetWrite();

	dd::Random rngPos( 0, 100 );
	dd::Random rngVelocity( 0, 100 );

	dd::Octree octree;

	// create a bunch of entities and components
	for( int i = 0; i < 1000; ++i )
	{
		dd::EntityHandle entity = entitySystem.CreateEntity();

		dd::SwarmAgentComponent* swarm_cmp = swarm_pool.Create( entity );
		dd::TransformComponent* transform_cmp = transform_pool.Create( entity );
		dd::OctreeComponent* octree_cmp = octree_pool.Create( entity );

		swarm_cmp->ID = i;
		swarm_cmp->Velocity = dd::Vector4( rngVelocity.Next() / (float) 100, rngVelocity.Next() / (float) 100, rngVelocity.Next() / (float) 100 );
		transform_cmp->Position = dd::Vector4( (float) rngPos.Next(), (float) rngPos.Next(), (float) rngPos.Next() );

		octree_cmp->Entry = octree.Add( transform_cmp->Position );
	}

	// copy them over
	transform_db.Swap();
	swarm_db.Swap();
	octree_db.Swap();

	transform_db.Duplicate();
	swarm_db.Duplicate();
	octree_db.Duplicate();

	dd::SwarmSystem swarm_system( swarm_db );

	dd::Window window( 640, 480, "Neutrino" );

	dd::Input input( window );

	while( !window.ShouldClose() )
	{
		input.Update();

		dd::MousePosition mouse_pos = input.GetMousePosition();

		dd::Array<dd::InputEvent, 64> events;
		input.GetKeyEvents( events );

		for( int i = 0; i < events.Size(); ++i )
		{
			if( events[ i ].Type == dd::InputType::PRESSED )
				std::cout << "Pressed a key!" << std::endl;

			if( events[ i ].Type == dd::InputType::RELEASED )
				std::cout << "Released a key!" << std::endl;
		}

		window.Swap();
	}

	window.Close();
	
	ASSERT( false, "DONE!" );
	return 0;
#endif
}