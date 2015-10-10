//
// Main.cpp - Entry point into application.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#include "PrecompiledHeader.h"

#include "CommandLine.h"
#include "Services.h"

#ifdef _TEST

#include "Tests.h"

#endif

#include "EntitySystem.h"
#include "TransformComponent.h"
#include "ScopedTimer.h"
#include "OctreeComponent.h"
#include "Recorder.h"
#include "PropertyList.h"
#include "DoubleBuffer.h"
#include "SwarmAgentComponent.h"
#include "SwarmSystem.h"
#include "Random.h"
#include "Window.h"
#include "Input.h"
#include "DebugUI.h"
#include "Timer.h"

#include "imgui/imgui.h"
//---------------------------------------------------------------------------

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

bool s_bDrawFPS;

void DrawFPS( float delta_t )
{
	ImGui::SetNextWindowPos( ImVec2( 2, 2 ) );
	if( !ImGui::Begin( "", &s_bDrawFPS, ImVec2( 0, 0 ), 0.4f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings ) )
	{
		ImGui::End();
		return;
	}
	ImGui::Text( "FPS: %.1f", 1.0f / delta_t );
	ImGui::End();
}

#ifdef _TEST

int TestMain( int argc, char* const argv[] )
{
	int iError = tests::RunTests( argc, argv );

	if( iError != 0 )
		ASSERT( false, "Tests failed!" );
	else
		printf( "Tests passed!" );

	return iError;
}

#endif

int GameMain()
{
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

	dd::Random32 rngPos( 0, 100 );
	dd::Random32 rngVelocity( 0, 100 );

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

	dd::Window window( 1280, 960, "Neutrino" );

	dd::Input input( window );

	bool opened;

	dd::DebugUI debugUI( window.GetInternalWindow() );

	input.AddKeyboardCallback( &dd::DebugUI::KeyCallback );
	input.AddScrollCallback( &dd::DebugUI::ScrollCallback );
	input.AddMouseCallback( &dd::DebugUI::MouseButtonCallback );
	input.AddCharCallback( &dd::DebugUI::CharCallback );

	dd::Timer timer;
	timer.Start();

	float last_frame = 0.0f;
	float current_frame = 0.0f;
	float delta_t = 0.0f;

	const float min_delta = 0.01f; // 100 FPS cap

	while( !window.ShouldClose() )
	{
		last_frame = current_frame;
		current_frame = timer.Time();
		delta_t = current_frame - last_frame;

		input.Update();

		dd::MousePosition mouse_pos = input.GetMousePosition();

		dd::Array<dd::InputEvent, 64> events;
		input.GetKeyEvents( events );

		for( uint i = 0; i < events.Size(); ++i )
		{
			if( events[i].Type == dd::InputType::PRESSED )
				std::cout << "Pressed a key!" << std::endl;

			if( events[i].Type == dd::InputType::RELEASED )
				std::cout << "Released a key!" << std::endl;
		}

		debugUI.SetMousePosition( input.GetMousePosition().X, input.GetMousePosition().Y );
		debugUI.SetFocused( window.IsFocused());
		debugUI.SetDisplaySize( window.GetWidth(), window.GetHeight() );

		debugUI.Update( delta_t );

		ImGui::ShowTestWindow( &opened );

		DrawFPS( delta_t );

		ImGui::Render();

		window.Swap();

		while( (timer.Time() - last_frame) < min_delta )
			::Sleep( 1 );
	}

	window.Close();

	ASSERT( false, "DONE!" );
	return 0;
}

//
// ENTRY POINT
//
int main( int argc, char* const argv[] )
{
	dd::CommandLine cmdLine( argv, argc );

	g_services.Register<dd::CommandLine>( &cmdLine );

	if( cmdLine.Exists( "noassert" ) )
		pempek::assert::implementation::ignoreAllAsserts( true );

#ifdef _TEST
	return TestMain( argc, argv );
#else
	return GameMain();
#endif
}