//
// Main.cpp - Entry point into application.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#include "PrecompiledHeader.h"

#include "CommandLine.h"

#ifdef _TEST

#include "Tests.h"

#endif

#include "Camera.h"
#include "DebugUI.h"
#include "DoubleBuffer.h"
#include "EntityManager.h"
#include "File.h"
#include "FrameTimer.h"
#include "FreeCameraController.h"
#include "IDebugDraw.h"
#include "Input.h"
#include "InputBindings.h"
#include "JobSystem.h"
#include "MeshComponent.h"
#include "Message.h"
#include "MousePicking.h"
#include "OctreeComponent.h"
#include "Random.h"
#include "Recorder.h"
#include "Renderer.h"
#include "SceneGraphSystem.h"
#include "ScopedTimer.h"
#include "ScriptComponent.h"
#include "ShipComponent.h"
#include "ShipSystem.h"
#include "StringBinding.h"
#include "SwarmAgentComponent.h"
#include "SwarmSystem.h"
#include "TerrainSystem.h"
#include "Timer.h"
#include "TransformComponent.h"
#include "TrenchSystem.h"
#include "Window.h"

#include "DebugConsole.h"

#include "imgui/imgui.h"

#include "Remotery/lib/Remotery.h"

#include "glm/gtc/matrix_transform.hpp"

#include "GL/gl3w.h"

#include "SFML/Network/UdpSocket.hpp"
//---------------------------------------------------------------------------

using namespace dd;

extern uint s_maxFPS;
uint s_maxFPS = 60;

bool s_showDebugUI = false;

FreeCameraController* s_freeCam;
ShipSystem* s_shipSystem;
Window* s_window;
EntityManager* s_entityManager;

#define REGISTER_GLOBAL_VARIABLE( engine, var ) engine.RegisterGlobalVariable<decltype(var), var>( #var )

TransformComponent* GetTransformComponent( EntityHandle entity )
{
	return entity.Get<TransformComponent>().Write();
}

EntityHandle GetEntityHandle( uint id )
{
	EntityHandle handle( id, *s_entityManager );
	return handle;
}

void RegisterGlobalScriptFunctions( AngelScriptEngine& script_engine )
{
	script_engine.RegisterFunction<decltype(&GetTransformComponent), &GetTransformComponent>( "GetTransformComponent" );
	script_engine.RegisterFunction<decltype(&GetEntityHandle), &GetEntityHandle>( "GetEntityHandle" );

	REGISTER_GLOBAL_VARIABLE( script_engine, s_maxFPS );
}

void RegisterGameTypes( EntityManager& entityManager, AngelScriptEngine& scriptEngine )
{
	dd::RegisterString( scriptEngine );

	REGISTER_POD( glm::vec3 );
	TypeInfo* vec3Type = TypeInfo::AccessType<glm::vec3>();
	vec3Type->RegisterScriptType<glm::vec3, true>();
	vec3Type->RegisterMember<glm::vec3, float, &glm::vec3::x>( "x" );
	vec3Type->RegisterMember<glm::vec3, float, &glm::vec3::y>( "y" );
	vec3Type->RegisterMember<glm::vec3, float, &glm::vec3::z>( "z" );

	REGISTER_POD( glm::vec4 );
	TypeInfo* vec4Type = TypeInfo::AccessType<glm::vec4>();
	vec4Type->RegisterScriptType<glm::vec4, true>();
	vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::x>( "x" );
	vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::y>( "y" );
	vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::z>( "z" );
	vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::z>( "w" );

	REGISTER_POD( glm::mat4 );

	REGISTER_TYPE( EntityHandle );
	REGISTER_TYPE( ComponentBase );
	REGISTER_TYPE( Message );
	REGISTER_TYPE( JobSystem );
	REGISTER_TYPE( MeshHandle );

	TypeInfo::RegisterComponent<TransformComponent>( "TransformComponent" );
	entityManager.RegisterComponent<TransformComponent>();

	TypeInfo::RegisterComponent<OctreeComponent>( "OctreeComponent" );
	entityManager.RegisterComponent<OctreeComponent>();

	TypeInfo::RegisterComponent<SwarmAgentComponent>( "SwarmAgentComponent" );
	entityManager.RegisterComponent<SwarmAgentComponent>();

	TypeInfo::RegisterComponent<MeshComponent>( "MeshComponent" );
	entityManager.RegisterComponent<MeshComponent>();

	TypeInfo::RegisterComponent<ShipComponent>( "ShipComponent" );
	entityManager.RegisterComponent<ShipComponent>();

	TypeInfo::RegisterComponent<ScriptComponent>( "ScriptComponent" );
	entityManager.RegisterComponent<ScriptComponent>();
}

void ToggleConsole( InputAction action, InputType type )
{
	if( action == InputAction::TOGGLE_CONSOLE && type == InputType::RELEASED )
	{
		s_showDebugUI = true;
	}
}

void ToggleFreeCam( InputAction action, InputType type )
{
	if( action == InputAction::TOGGLE_FREECAM && type == InputType::RELEASED )
	{
		s_freeCam->Enable( !s_freeCam->IsEnabled() );
		s_shipSystem->Enable( !s_shipSystem->IsEnabled() );
	}
}

void ToggleDebugUI( InputAction action, InputType type )
{
	if( action == InputAction::TOGGLE_DEBUG_UI && type == InputType::RELEASED )
	{
		s_showDebugUI = !s_showDebugUI;
	}
}

void Exit( InputAction action, InputType type )
{
	if( action == InputAction::EXIT && type == InputType::RELEASED )
	{
		s_window->SetToClose();
	}
}

void BindKeys( Input& input )
{
	input.BindKey( Input::Key::F1, InputAction::TOGGLE_FREECAM );
	input.BindKey( Input::Key::F2, InputAction::TOGGLE_CONSOLE );
	input.BindKey( Input::Key::ESCAPE, InputAction::TOGGLE_DEBUG_UI );
	input.BindKey( 'W', InputAction::FORWARD );
	input.BindKey( 'S', InputAction::BACKWARD );
	input.BindKey( 'A', InputAction::LEFT );
	input.BindKey( 'D', InputAction::RIGHT );
	input.BindKey( ' ', InputAction::UP );
	input.BindKey( Input::Key::LCTRL, InputAction::DOWN );
	input.BindKey( Input::Key::LSHIFT, InputAction::BOOST );
	input.BindMouseButton( Input::MouseButton::LEFT, InputAction::SELECT_MESH );
}

void UpdateFreeCam( FreeCameraController& free_cam, Input& input, float delta_t )
{
	bool captureMouse = !s_showDebugUI;
	if( captureMouse != input.IsMouseCaptured() )
	{
		input.CaptureMouse( captureMouse );
	}

	if( captureMouse )
	{
		free_cam.UpdateMouse( input.GetMousePosition() );
		free_cam.UpdateScroll( input.GetScrollPosition() );
		free_cam.Update( delta_t );
	}
}

void PreUpdateSystems( JobSystem& jobsystem, EntityManager& entity_manager, Vector<ISystem*>& systems, float delta_t )
{
	for( ISystem* system : systems )
	{
		jobsystem.Schedule( [&entity_manager, system, delta_t]() { system->PreUpdate( entity_manager, delta_t ); }, "System::PreUpdate" );
	}

	jobsystem.WaitForCategory( "System::PreUpdate" );
}

void UpdateSystems( JobSystem& jobsystem, EntityManager& entity_manager, Vector<ISystem*>& systems, float delta_t )
{
	for( ISystem* system : systems )
	{
		jobsystem.Schedule( [&entity_manager, system, delta_t]() { system->Update( entity_manager, delta_t ); }, "System::Update" );
	}

	jobsystem.WaitForCategory( "System::Update" );
}

void PostRenderSystems( JobSystem& jobsystem, EntityManager& entity_manager, Vector<ISystem*>& systems, float delta_t )
{
	for( ISystem* system : systems )
	{
		jobsystem.Schedule( [&entity_manager, system, delta_t]() { system->PostRender( entity_manager, delta_t ); }, "System::PostRender" );
	}

	jobsystem.WaitForCategory( "System::PostRender" );
}

void DrawDebugUI( const Vector<IDebugDraw*>& views )
{
	if( s_showDebugUI )
	{
		if( ImGui::BeginMainMenuBar() )
		{
			if( ImGui::BeginMenu( "File" ) )
			{
				if( ImGui::MenuItem( "Exit" ) )
					s_window->SetToClose();

				ImGui::EndMenu();
			}

			if( ImGui::BeginMenu( "Views" ) )
			{
				for( IDebugDraw* debug_view : views )
				{
					ImGui::MenuItem( debug_view->GetDebugTitle(), nullptr, &debug_view->IsDebugOpen() );
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	for( IDebugDraw* debug_view : views )
	{
		if( debug_view->IsDebugOpen() )
			debug_view->DrawDebug();
	}
}

void Render( Renderer& renderer, EntityManager& entity_manager, DebugConsole& console, FrameTimer& frame_timer, float delta_t )
{
	renderer.Render( entity_manager, delta_t );

	ImGui::Render();

	s_window->Swap();
}

void UpdateInput( Input& input, InputBindings& bindings, float delta_t )
{
	Array<InputEvent, 64> events;
	input.Update( delta_t );

	// update input
	input.GetKeyEvents( events );
	bindings.Dispatch( events );
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

int GameMain( EntityManager& entityManager, AngelScriptEngine& scriptEngine )
{
	DD_PROFILE_INIT();
	DD_PROFILE_THREAD_NAME( "Main" );

	//::ShowWindow( GetConsoleWindow(), SW_HIDE );

	{
		JobSystem jobsystem( 2u );
		SwarmSystem swarm_system;

		s_window = new Window( 1280, 720, "DD" );
		Input input( *s_window );

		DebugUI debugUI( *s_window, input );

		Renderer renderer;
		renderer.Initialize( *s_window, entityManager );

		Camera& camera = renderer.GetCamera();
		camera.SetPosition( glm::vec3( 0, 5, 0 ) );
		camera.SetDirection( glm::vec3( 0, 0, 1 ) );

		//TerrainSystem terrain_system( camera );
		//terrain_system.Initialize( entity_manager );

		SceneGraphSystem scene_graph;

		TrenchSystem trench_system( camera );
		trench_system.CreateRenderResources();

		InputBindings bindings;
		bindings.RegisterHandler( InputAction::TOGGLE_FREECAM, &ToggleFreeCam );
		bindings.RegisterHandler( InputAction::TOGGLE_DEBUG_UI, &ToggleDebugUI );
		bindings.RegisterHandler( InputAction::EXIT, &Exit );

		s_freeCam = new FreeCameraController( camera );
		s_freeCam->BindActions( bindings );

		s_shipSystem = new ShipSystem( camera );
		s_shipSystem->BindActions( bindings );
		s_shipSystem->CreateShip( entityManager );

		MousePicking mouse_picking( *s_window, camera, input );
		mouse_picking.BindActions( bindings );
		renderer.SetMousePicking( &mouse_picking );

		Vector<ISystem*> systems;
		systems.Add( &scene_graph );
		systems.Add( &swarm_system );
		systems.Add( &trench_system );
		systems.Add( &mouse_picking );
		systems.Add( s_shipSystem );

		BindKeys( input );

		FrameTimer frame_timer;
		DebugConsole console( scriptEngine );

		Vector<IDebugDraw*> debug_views;
		debug_views.Add( &frame_timer );
		debug_views.Add( &console );
		debug_views.Add( &renderer );
		debug_views.Add( &mouse_picking );
		debug_views.Add( s_freeCam );
		debug_views.Add( s_shipSystem );

		while( !s_window->ShouldClose() )
		{
			DD_PROFILE_SCOPED( Frame );

			// frame timer
			frame_timer.SetMaxFPS( s_maxFPS );
			frame_timer.Update();
			float delta_t = frame_timer.Delta();

			// entity manager
			entityManager.Update( delta_t );

			// systems pre-update
			PreUpdateSystems( jobsystem, entityManager, systems, delta_t );

			// input
			UpdateInput( input, bindings, delta_t );

			// debug UI
			debugUI.Update( delta_t );

			// camera
			UpdateFreeCam( *s_freeCam, input, delta_t );

			// systems update
			UpdateSystems( jobsystem, entityManager, systems, delta_t );

			// debug UI
			DrawDebugUI( debug_views );

			// render
			Render( renderer, entityManager, console, frame_timer, delta_t );

			// systems post-render
			PostRenderSystems( jobsystem, entityManager, systems, delta_t );

			// wait for frame delta
			frame_timer.DelayFrame();

			DD_PROFILE_LOG( "End Frame" );
		}

		systems.Clear();
		renderer.Shutdown();
	}

	delete s_freeCam;
	delete s_shipSystem;

	s_window->Close();
	delete s_window;

	DD_PROFILE_DEINIT();

	return 0;
}

//
// ENTRY POINT
//
int main( int argc, char const* argv[] )
{
	TypeInfo::RegisterDefaultTypes();

	CommandLine cmdLine( argv, argc );
	if( cmdLine.Exists( "noassert" ) )
		pempek::assert::implementation::ignoreAllAsserts( true );

	if( cmdLine.Exists( "dataroot" ) )
		dd::File::SetDataRoot( cmdLine.GetValue( "dataroot" ).c_str() );
	else
		dd::File::SetDataRoot( "../../../data" );

	REGISTER_TYPE( CommandLine );

	// TODO: this is bad, not compatible with Wren, and registered too early anyway
	AngelScriptEngine scriptEngine;
	REGISTER_TYPE( AngelScriptEngine );

	EntityManager entityManager;
	REGISTER_TYPE( EntityManager );
	s_entityManager = &entityManager;

	TypeInfo::SetScriptEngine( &scriptEngine );

	RegisterGameTypes( entityManager, scriptEngine );
	RegisterGlobalScriptFunctions( scriptEngine );

#ifdef _TEST
	return TestMain( argc, argv );
#else
	return GameMain( entityManager, scriptEngine );
#endif
}
