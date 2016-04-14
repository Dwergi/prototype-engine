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

#include "Camera.h"
#include "DebugUI.h"
#include "DoubleBuffer.h"
#include "EntitySystem.h"
#include "FreeCameraController.h"
#include "Input.h"
#include "InputBindings.h"
#include "JobSystem.h"
#include "Message.h"
#include "OctreeComponent.h"
#include "Random.h"
#include "Recorder.h"
#include "Renderer.h"
#include "ScopedTimer.h"
#include "SwarmAgentComponent.h"
#include "SwarmSystem.h"
#include "Timer.h"
#include "TransformComponent.h"
#include "Window.h"

#include "DebugConsole.h"

#include "imgui/imgui.h"

#include "Remotery/lib/Remotery.h"

#include "glm/gtc/matrix_transform.hpp"

#include "GL/gl3w.h"
//---------------------------------------------------------------------------

using namespace dd;

extern bool s_drawFPS;
bool s_drawFPS = true;

extern float s_maxFPS;
float s_maxFPS = 60.0f;

extern float s_rollingAverageFPS;
float s_rollingAverageFPS = s_maxFPS;

extern float s_rollingAverageMultiplier;
float s_rollingAverageMultiplier = 0.9f;

bool s_drawConsole = false;

bool s_drawCameraDebug = true;

Window* s_window = nullptr;
Input* s_input = nullptr;

#define REGISTER_GLOBAL_VARIABLE( engine, var ) engine.RegisterGlobalVariable<decltype(var), var>( #var )

void DrawFPS( float delta_t )
{
	if( delta_t > 0 )
	{
		s_rollingAverageFPS *= s_rollingAverageMultiplier;
		s_rollingAverageFPS += (1.0f / delta_t) * (1.0f - s_rollingAverageMultiplier);
	}

	ImGui::SetNextWindowPos( ImVec2( 2, 2 ) );
	if( !ImGui::Begin( "FPS", &s_drawFPS, ImVec2( 0, 0 ), 0.4f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings ) )
	{
		ImGui::End();
		return;
	}

	ImGui::Text( "FPS: %.1f", s_rollingAverageFPS );
	ImGui::End();
}

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

	REGISTER_POD( glm::vec4 );
	TypeInfo* vec4Type = TypeInfo::AccessType<glm::vec4>();
	vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::x>( "x" );
	vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::y>( "y" );
	vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::z>( "z" );
	vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::z>( "w" );

	REGISTER_POD( glm::mat4 );

	REGISTER_TYPE( EntityHandle );
	REGISTER_TYPE( Component );
	REGISTER_TYPE( Message );
	REGISTER_TYPE( JobSystem );

	REGISTER_TYPE( TransformComponent );
	REGISTER_TYPE( OctreeComponent );
	REGISTER_TYPE( SwarmAgentComponent );

	Services::RegisterComponent<TransformComponent>();
	Services::RegisterComponent<OctreeComponent>();
	Services::RegisterComponent<SwarmAgentComponent>();

	REGISTER_TYPE( SwarmSystem );

	RegisterGlobalScriptFunctions();
}

void ToggleConsole( InputAction action, InputType type )
{
	if( action == InputAction::CONSOLE && type == InputType::RELEASED )
	{
		s_drawConsole = !s_drawConsole;

		s_input->CaptureMouse( !s_drawConsole );
	}
}

void BindKeys( Input& input )
{
	input.BindKey( '`', InputAction::CONSOLE );
	input.BindKey( 'W', InputAction::FORWARD );
	input.BindKey( 'S', InputAction::BACKWARD );
	input.BindKey( 'A', InputAction::LEFT );
	input.BindKey( 'D', InputAction::RIGHT );
	input.BindKey( ' ', InputAction::UP );
	input.BindKey( Input::Key::LCTRL, InputAction::DOWN );
	input.BindKey( Input::Key::LSHIFT, InputAction::BOOST );
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

	//::ShowWindow( GetConsoleWindow(), SW_HIDE );

	EntitySystem entitySystem;
	REGISTER_TYPE( EntitySystem );
	Services::Register( entitySystem );

	JobSystem jobsystem( 2u );

	auto& transform_db = Services::GetDoubleBuffer<TransformComponent>();
	auto& swarm_db = Services::GetDoubleBuffer<SwarmAgentComponent>();
	auto& octree_db = Services::GetDoubleBuffer<OctreeComponent>();

	SwarmSystem swarm_system;
	Services::Register( swarm_system );

	{
		s_window = new Window( 1280, 960, "Neutrino" );
		s_input = new Input( *s_window );
		s_input->CaptureMouse( !s_drawConsole );

		DebugUI debugUI( *s_window, *s_input );

		DebugConsole console;
		//bool opened;

		Timer timer;
		timer.Start();

		float target_delta = 1.0f / s_maxFPS;
		float last_frame = 0.0f;
		float current_frame = -target_delta;
		float delta_t = target_delta;

		BindKeys( *s_input );

		Renderer renderer( *s_window );
		renderer.Init();
		
		FreeCameraController free_cam( renderer.GetCamera() );

		InputBindings bindings;
		bindings.RegisterHandler( InputAction::CONSOLE, &ToggleConsole );

		auto handle_input = std::bind( &FreeCameraController::HandleInput, std::ref( free_cam ), std::placeholders::_1, std::placeholders::_2 );
		bindings.RegisterHandler( InputAction::FORWARD, handle_input );
		bindings.RegisterHandler( InputAction::BACKWARD, handle_input );
		bindings.RegisterHandler( InputAction::LEFT, handle_input );
		bindings.RegisterHandler( InputAction::RIGHT, handle_input );
		bindings.RegisterHandler( InputAction::UP, handle_input );
		bindings.RegisterHandler( InputAction::DOWN, handle_input );
		bindings.RegisterHandler( InputAction::BOOST, handle_input );

		while( !s_window->ShouldClose() )
		{
			DD_PROFILE_START( Frame );

			target_delta = 1.0f / s_maxFPS;
			last_frame = current_frame;
			current_frame = timer.Time();
			delta_t = current_frame - last_frame;

			s_input->Update( delta_t );

			// update input
			Array<InputEvent, 64> events;
			s_input->GetKeyEvents( events );
			bindings.Dispatch( events );

			debugUI.Update( delta_t );

			free_cam.UpdateMouse( s_input->GetMousePosition() );
			free_cam.UpdateScroll( s_input->GetScrollPosition() );
			free_cam.Update( delta_t );

			if( s_drawCameraDebug )
				free_cam.DrawCameraDebug();

			jobsystem.Schedule( std::bind( &SwarmSystem::Update, Services::GetPtr<SwarmSystem>(), delta_t ), "SwarmSystem" );

			renderer.Render( delta_t );

			if( s_drawConsole )
				console.Draw( "Console", s_drawConsole );

			if( s_drawFPS )
				DrawFPS( delta_t );

			ImGui::Render();

			s_window->Swap();

			DD_PROFILE_START( Main_Sleep );

			float now = timer.Time();
			while( now - last_frame < target_delta )
			{
				::Sleep( 0 );

				now = timer.Time();
			}

			DD_PROFILE_END();

			DD_PROFILE_END();

			DD_PROFILE_LOG( "End Frame" );
		}

		s_window->Close();
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
