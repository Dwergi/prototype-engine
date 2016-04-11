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

#include "AABB.h"
#include "DebugUI.h"
#include "DoubleBuffer.h"
#include "EntitySystem.h"
#include "Input.h"
#include "JobSystem.h"
#include "OctreeComponent.h"
#include "PropertyList.h"
#include "Random.h"
#include "Recorder.h"
#include "ScopedTimer.h"
#include "SwarmAgentComponent.h"
#include "SwarmSystem.h"
#include "Timer.h"
#include "TransformComponent.h"
#include "Window.h"

#include "DebugConsole.h"

#include "imgui/imgui.h"

#include "Remotery/lib/Remotery.h"
//---------------------------------------------------------------------------

using namespace dd;

extern bool s_drawFPS;
bool s_drawFPS = true;

extern float s_maxFPS;
float s_maxFPS = 30.0f;

extern float s_rollingAverageFPS;
float s_rollingAverageFPS = s_maxFPS;

extern float s_rollingAverageMultiplier;
float s_rollingAverageMultiplier = 0.8f;

#define REGISTER_GLOBAL_VARIABLE( engine, var ) engine.RegisterGlobalVariable<decltype(var), var>( #var )

Vector<EntityHandle> s_entitites;

void DrawFPS( float delta_t )
{
	if( delta_t > 0 )
	{
		s_rollingAverageFPS *= s_rollingAverageMultiplier;
		s_rollingAverageFPS += (1.0f / delta_t) * (1.0f - s_rollingAverageMultiplier);
	}

	ImGui::SetNextWindowPos( ImVec2( 2, 2 ) );
	if( !ImGui::Begin( "", &s_drawFPS, ImVec2( 0, 0 ), 0.4f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings ) )
	{
		ImGui::End();
		return;
	}

	ImGui::Text( "FPS: %.1f", s_rollingAverageFPS );
	ImGui::End();
}

bool s_drawConsole = true;

TransformComponent* GetTransformComponent( EntityHandle entity )
{
	auto& transform_pool = Services::GetReadPool<TransformComponent>();
	
	TransformComponent* cmp = transform_pool.Find( entity );

	return cmp;
}

EntityHandle GetEntityHandle( uint id )
{
	EntitySystem& system = Services::Get<EntitySystem>();

	EntityHandle handle( id, &system );
	return handle;
}

void RegisterGlobalScriptFunctions()
{
	REGISTER_TYPE( EntityHandle );

	ScriptEngine& engine = Services::Get<ScriptEngine>();

	engine.RegisterFunction<decltype(&GetTransformComponent), &GetTransformComponent>( "GetTransformComponent" );
	engine.RegisterFunction<decltype(&GetEntityHandle), &GetEntityHandle>( "GetEntityHandle" );

	REGISTER_GLOBAL_VARIABLE( engine, s_drawFPS );
	REGISTER_GLOBAL_VARIABLE( engine, s_maxFPS );
	REGISTER_GLOBAL_VARIABLE( engine, s_rollingAverageFPS );
	REGISTER_GLOBAL_VARIABLE( engine, s_rollingAverageMultiplier );
}

void RegisterGameTypes()
{
	REGISTER_POD( glm::vec3 );
	TypeInfo* vec3Type = TypeInfo::AccessType<glm::vec3>();
	vec3Type->RegisterMember<glm::vec3, float, &glm::vec3::x>( "x" );
	vec3Type->RegisterMember<glm::vec3, float, &glm::vec3::y>( "y" );
	vec3Type->RegisterMember<glm::vec3, float, &glm::vec3::z>( "z" );

	REGISTER_TYPE( EntityHandle );
	REGISTER_TYPE( Component );
	REGISTER_TYPE( TransformComponent );
	REGISTER_TYPE( OctreeComponent );
	REGISTER_TYPE( SwarmAgentComponent );

	Services::RegisterComponent<TransformComponent>();
	Services::RegisterComponent<OctreeComponent>();
	Services::RegisterComponent<SwarmAgentComponent>();

	REGISTER_TYPE( SwarmSystem );

	RegisterGlobalScriptFunctions();
}

#ifdef _TEST

int TestMain( int argc, char const* argv[] )
{
	int iError = tests::RunTests( argc, argv );

	if( iError != 0 )
		DD_ASSERT( false, "Tests failed!" );
	else
		printf( "Tests passed!" );

	return iError;
}

#endif

int GameMain()
{
	DD_PROFILE_INIT();
	DD_PROFILE_THREAD_NAME( "Main" );

	::ShowWindow( GetConsoleWindow(), SW_HIDE );

	EntitySystem entitySystem;
	Services::Register( entitySystem );

	RegisterGameTypes();

	JobSystem jobsystem( 2u );

	auto& transform_db = Services::GetDoubleBuffer<TransformComponent>();
	auto& swarm_db = Services::GetDoubleBuffer<SwarmAgentComponent>();
	auto& octree_db = Services::GetDoubleBuffer<OctreeComponent>();

	auto& transform_pool = transform_db.GetWrite();
	auto& swarm_pool = swarm_db.GetWrite();
	auto& octree_pool = octree_db.GetWrite();

	Random32 rngPos( 0, 100, 50 );
	Random32 rngVelocity( 0, 100 );

	AABBOctree octree;

	// create a bunch of entities and components
	for( int i = 0; i < 1000; ++i )
	{
		EntityHandle entity = entitySystem.Create();

		SwarmAgentComponent* swarm_cmp = swarm_pool.Create( entity );
		TransformComponent* transform_cmp = transform_pool.Create( entity );
		OctreeComponent* octree_cmp = octree_pool.Create( entity );

		swarm_cmp->Velocity = glm::vec3( rngVelocity.Next() / (float) 100, rngVelocity.Next() / (float) 100, rngVelocity.Next() / (float) 100 );
		transform_cmp->Position = glm::vec3( (float) rngPos.Next(), (float) rngPos.Next(), (float) rngPos.Next() );

		AABB aabb;
		aabb.Expand( glm::vec3( transform_cmp->Position.x, transform_cmp->Position.y, transform_cmp->Position.z ) );
		octree_cmp->Entry = octree.Add( aabb );
	}

	// copy them over
	transform_db.Swap();
	swarm_db.Swap();
	octree_db.Swap();

	transform_db.Duplicate();
	swarm_db.Duplicate();
	octree_db.Duplicate();

	SwarmSystem swarm_system;
	Services::Register( swarm_system );

	{
		Window window( 1280, 960, "Neutrino" );

		Input input( window );

		bool opened;

		DebugUI debugUI( window.GetInternalWindow() );

		DebugConsole console;

		input.AddKeyboardCallback( &DebugUI::KeyCallback );
		input.AddScrollCallback( &DebugUI::ScrollCallback );
		input.AddMouseCallback( &DebugUI::MouseButtonCallback );
		input.AddCharCallback( &DebugUI::CharCallback );

		Timer timer;
		timer.Start();

		float target_delta = 1.0f / s_maxFPS;
		float last_frame = 0.0f;
		float current_frame = -target_delta;
		float delta_t = target_delta;

		input.BindKey( '`', InputAction::CONSOLE );

		while( !window.ShouldClose() )
		{
			DD_PROFILE_START( Frame );

			target_delta = 1.0f / s_maxFPS;
			last_frame = current_frame;
			current_frame = timer.Time();
			delta_t = current_frame - last_frame;

			input.Update();

			MousePosition mouse_pos = input.GetMousePosition();

			Array<InputEvent, 64> events;
			input.GetKeyEvents( events );

			for( uint i = 0; i < events.Size(); ++i )
			{
				if( events[i].Action == InputAction::CONSOLE && events[i].Type == InputType::RELEASED )
					s_drawConsole = !s_drawConsole;
			}

			debugUI.SetMousePosition( input.GetMousePosition().X, input.GetMousePosition().Y );
			debugUI.SetFocused( window.IsFocused() );
			debugUI.SetDisplaySize( window.GetWidth(), window.GetHeight() );

			debugUI.Update( delta_t );

			jobsystem.Schedule( std::bind( &SwarmSystem::Update, Services::GetPtr<SwarmSystem>(), delta_t ), "SwarmSystem" );

			ImGui::ShowTestWindow( &opened );

			if( s_drawConsole )
				console.Draw( "Console", s_drawConsole );

			if( s_drawFPS )
				DrawFPS( delta_t );

			ImGui::Render();

			window.Swap();

			DD_PROFILE_START( Main_Sleep );

			float now = timer.Time();
			while( now - last_frame < target_delta )
			{
				::Sleep( 1 );

				now = timer.Time();
			}

			DD_PROFILE_END();

			DD_PROFILE_END();

			DD_PROFILE_LOG( "End Frame" );
		}

		window.Close();
	}

	DD_PROFILE_DEINIT(); 

	DD_ASSERT( false, "DONE!" );
	return 0;
}

//
// ENTRY POINT
//
int main( int argc, char const* argv[] )
{
	Services::Initialize();

	TypeInfo::RegisterDefaultTypes();

	CommandLine cmdLine( argv, argc );
	if( cmdLine.Exists( "noassert" ) )
		pempek::assert::implementation::ignoreAllAsserts( true );

	REGISTER_TYPE( CommandLine );
	Services::Register( cmdLine );

	// TODO: this is bad, not compatible with Wren, and registered too early anyway
	REGISTER_TYPE( ScriptEngine );

	ScriptEngine scriptEngine;
	Services::Register( scriptEngine );

	RegisterGameTypes();

#ifdef _TEST
	return TestMain( argc, argv );
#else
	return GameMain();
#endif
}
