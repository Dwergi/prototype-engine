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

#include "DebugUI.h"
#include "DDAssertHelpers.h"
#include "DoubleBuffer.h"
#include "EntityManager.h"
#include "File.h"
#include "FrameTimer.h"
#include "FreeCameraController.h"
#include "FSM.h"
#include "FPSCamera.h"
#include "GLError.h"
#include "IDebugPanel.h"
#include "Input.h"
#include "InputBindings.h"
#include "JobSystem.h"
#include "LightComponent.h"
#include "MeshComponent.h"
#include "MeshRenderer.h"
#include "Message.h"
#include "MousePicking.h"
#include "OctreeComponent.h"
#include "Random.h"
#include "Recorder.h"
#include "Renderer.h"
#include "FrameBuffer.h"
#include "ParticleEmitter.h"
#include "SceneGraphSystem.h"
#include "ScopedTimer.h"
#include "ScriptComponent.h"
#include "ShakyCamera.h"
#include "ShipComponent.h"
#include "ShipSystem.h"
#include "StringBinding.h"
#include "SwarmAgentComponent.h"
#include "SwarmSystem.h"
#include "TerrainChunkComponent.h"
#include "TerrainSystem.h"
#include "Texture.h"
#include "Timer.h"
#include "TransformComponent.h"
#include "TrenchSystem.h"
#include "Uniforms.h"
#include "Window.h"

#include "DebugConsole.h"

#include "imgui/imgui.h"

#include "Remotery/lib/Remotery.h"

#include "glm/gtc/matrix_transform.hpp"

#include "GL/gl3w.h"

#include "SFML/Network/UdpSocket.hpp"

#include <chrono>
#include <thread>
//---------------------------------------------------------------------------

using namespace dd;

extern uint s_maxFPS;
uint s_maxFPS = 60;

bool s_showDebugUI = false;
DebugUI* s_debugUI = nullptr;

Input* s_input = nullptr;
FreeCameraController* s_freeCam = nullptr;
ShipSystem* s_shipSystem = nullptr;
Window* s_window = nullptr;
EntityManager* s_entityManager = nullptr;
FrameTimer* s_frameTimer = nullptr;
FSM* s_fsm = nullptr;
Assert s_assert;

std::thread::id s_mainThread;

enum FSMStates
{
	INITIALIZED,
	UPDATE_TIMER,
	PREUPDATE,
	UPDATE,
	RENDER,
	RENDER_END_FRAME,
	POSTRENDER,
	OPEN_ASSERT,
	ASSERT_DIALOG
};

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

void RegisterGameTypes( EntityManager& entity_manager, AngelScriptEngine& scriptEngine )
{
	RegisterString( scriptEngine );

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
	REGISTER_TYPE( IComponent );
	REGISTER_TYPE( Message );
	REGISTER_TYPE( ddr::MeshHandle );

	TypeInfo::RegisterComponent<TransformComponent>( "TransformComponent" );
	entity_manager.RegisterComponent<TransformComponent>();

	TypeInfo::RegisterComponent<OctreeComponent>( "OctreeComponent" );
	entity_manager.RegisterComponent<OctreeComponent>();

	TypeInfo::RegisterComponent<SwarmAgentComponent>( "SwarmAgentComponent" );
	entity_manager.RegisterComponent<SwarmAgentComponent>();

	TypeInfo::RegisterComponent<MeshComponent>( "MeshComponent" );
	entity_manager.RegisterComponent<MeshComponent>();

	TypeInfo::RegisterComponent<ShipComponent>( "ShipComponent" );
	entity_manager.RegisterComponent<ShipComponent>();

	TypeInfo::RegisterComponent<ScriptComponent>( "ScriptComponent" );
	entity_manager.RegisterComponent<ScriptComponent>();

	TypeInfo::RegisterComponent<LightComponent>( "LightComponent" );
	entity_manager.RegisterComponent<LightComponent>();

	TypeInfo::RegisterComponent<TerrainChunkComponent>( "TerrainChunkComponent" );
	entity_manager.RegisterComponent<TerrainChunkComponent>();
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

void TriggerAssert( InputAction action, InputType type )
{
	if( action == InputAction::BREAK && type == InputType::RELEASED )
	{
		DD_ASSERT( false );
	}
}

void Exit( InputAction action, InputType type )
{
	if( action == InputAction::EXIT && type == InputType::RELEASED )
	{
		s_window->SetToClose();
	}
}

void CheckAssert()
{
	if( s_assert.Open )
	{
		s_input->CaptureMouse( false );

		s_fsm->TransitionTo( OPEN_ASSERT );

		do
		{
			s_fsm->TransitionTo( UPDATE_TIMER );
			s_fsm->TransitionTo( ASSERT_DIALOG );
			s_fsm->TransitionTo( RENDER_END_FRAME );
		} 
		while( s_assert.Open );
	}
}

pempek::assert::implementation::AssertAction::AssertAction OnAssert( const char* file, int line, const char* function, const char* expression,
	int level, const char* message )
{
	s_assert.Open = true;
	s_assert.Info = FormatAssert( level, file, line, function, expression );
	s_assert.Message = String256();
	s_assert.Action = AssertAction::None;
	if( message != nullptr )
	{
		s_assert.Message += "Message: ";
		s_assert.Message += message;
	}

	do
	{
		if( std::this_thread::get_id() == s_mainThread )
		{
			CheckAssert();
		}
		else
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
		}
	} 
	while( s_assert.Action == AssertAction::None );

	return (pempek::assert::implementation::AssertAction::AssertAction) s_assert.Action;
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
	input.BindKey( 'R', InputAction::ADD_MINOR_TRAUMA );
	input.BindKey( 'T', InputAction::ADD_MAJOR_TRAUMA );
	input.BindKey( 'P', InputAction::TOGGLE_PICKING );
	input.BindKey( Input::Key::LCTRL, InputAction::DOWN );
	input.BindKey( Input::Key::LSHIFT, InputAction::BOOST );
	input.BindMouseButton( Input::MouseButton::LEFT, InputAction::SELECT_MESH );
	input.BindKey( Input::Key::PAUSE, InputAction::BREAK );
}

void UpdateFreeCam( FreeCameraController& free_cam, ShakyCamera& shaky_cam, Input& input, float delta_t )
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

	shaky_cam.Update( delta_t );
}

void WaitForAll( std::vector<std::future<void>>& futures )
{
	size_t ready = 0;

	while( ready < futures.size() )
	{
		for( std::future<void>& f : futures )
		{
			std::future_status s = f.wait_for( std::chrono::microseconds( 1 ) );

			if( s == std::future_status::ready )
			{
				++ready;
			}
		}
	}
}

void InitializeSystems( JobSystem& jobsystem, EntityManager& entity_manager, const Vector<ISystem*>& systems )
{
	std::vector<std::future<void>> futures;

	for( ISystem* system : systems )
	{
		futures.push_back( jobsystem.Schedule( [&entity_manager, system]() { system->Initialize( entity_manager ); } ) );
	}

	WaitForAll( futures );
}

void InitializeRenderers( ddr::Renderer& renderer, const Vector<IRenderer*>& renderers )
{
	renderer.RenderInit();

	for( IRenderer* current : renderers )
	{
		current->RenderInit();
	}
}

void PreUpdateSystems( JobSystem& jobsystem, EntityManager& entity_manager, const Vector<ISystem*>& systems, float delta_t )
{
	std::vector<std::future<void>> futures;
	for( ISystem* system : systems )
	{
		futures.push_back( jobsystem.Schedule( [&entity_manager, system, delta_t]() { system->PreUpdate( entity_manager, delta_t ); } ) );
	}

	WaitForAll( futures );
}

void UpdateSystems( JobSystem& jobsystem, EntityManager& entity_manager, const Vector<ISystem*>& systems, float delta_t )
{
	std::vector<std::future<void>> futures;
	for( ISystem* system : systems )
	{
		futures.push_back( jobsystem.Schedule( [&entity_manager, system, delta_t]() { system->Update( entity_manager, delta_t ); } ) );
	}

	WaitForAll( futures );
}

void PostRenderSystems( JobSystem& jobsystem, EntityManager& entity_manager, const Vector<ISystem*>& systems, float delta_t )
{
	std::vector<std::future<void>> futures;
	for( ISystem* system : systems )
	{
		futures.push_back( jobsystem.Schedule( [&entity_manager, system, delta_t]() { system->PostRender( entity_manager, delta_t ); } ) );
	}

	WaitForAll( futures );
}

void ShutdownSystems( EntityManager& entity_manager, const Vector<ISystem*>& systems )
{
	for( ISystem* system : systems )
	{
		system->Shutdown( entity_manager );
	}
}

void DrawDebugUI( const Vector<IDebugPanel*>& views )
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
				for( IDebugPanel* debug_view : views )
				{
					debug_view->AddToMenu();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	for( IDebugPanel* debug_view : views )
	{
		if( debug_view->IsDebugPanelOpen() )
			debug_view->DrawDebugPanel();
	}

	s_frameTimer->DrawFPSCounter();
}

void Render( ddr::Renderer& renderer, const Vector<IRenderer*>& renderers, EntityManager& entity_manager, const ICamera& camera, ddr::UniformStorage& uniforms, float delta_t )
{
	IRenderer* debug_render = nullptr;

	renderer.BeginRender( entity_manager, camera, uniforms );

	for( IRenderer* r : renderers )
	{
		if( !r->UsesAlpha() )
		{
			r->Render( entity_manager, camera, uniforms );
		}

		if( r->ShouldRenderDebug() )
		{
			debug_render = r;
			break;
		}
	}

	if( debug_render != nullptr )
	{
		renderer.RenderDebug( *debug_render );
	}

	for( IRenderer* r : renderers )
	{
		if( r->UsesAlpha() )
		{
			r->Render( entity_manager, camera, uniforms );
		}
	}
}

void UpdateInput( Input& input, InputBindings& bindings, float delta_t )
{
	input.Update( delta_t );

	// update input
	Array<InputEvent, 64> events;
	input.GetKeyEvents( events );
	bindings.Dispatch( events );
}

int GameMain( EntityManager& entity_manager, AngelScriptEngine& scriptEngine )
{
	DD_PROFILE_INIT();
	DD_PROFILE_THREAD_NAME( "Main" );

	s_mainThread = std::this_thread::get_id();

	{
		//SwarmSystem swarm_system;

		s_window = new Window( glm::ivec2( 1280, 720 ), "DD" );
		s_input = new Input( *s_window );

		InputBindings bindings;
		bindings.RegisterHandler( InputAction::TOGGLE_FREECAM, &ToggleFreeCam );
		bindings.RegisterHandler( InputAction::TOGGLE_DEBUG_UI, &ToggleDebugUI );
		bindings.RegisterHandler( InputAction::EXIT, &Exit );
		bindings.RegisterHandler( InputAction::BREAK, &TriggerAssert );

		FPSCamera camera( *s_window );
		camera.SetPosition( glm::vec3( 0, 10, 0 ) );
		camera.SetDirection( glm::vec3( 0, 0, 1 ) );

		ShakyCamera shaky_cam( camera, bindings );

		s_debugUI = new DebugUI( *s_window, *s_input );

		ddr::Renderer renderer( *s_window );
		ddr::UniformStorage uniforms;

		JobSystem jobSystem( 7u );

		TerrainSystem terrain_system( jobSystem );

		ddr::ParticleEmitter particle_system;

		SceneGraphSystem scene_graph;

		//TrenchSystem trench_system( camera );
		//trench_system.CreateRenderResources();

		s_freeCam = new FreeCameraController( camera );
		s_freeCam->BindActions( bindings );

		//s_shipSystem = new ShipSystem( camera );
		//s_shipSystem->BindActions( bindings );
		//s_shipSystem->CreateShip( entity_manager );

		MousePicking mouse_picking( *s_window, *s_input );
		mouse_picking.BindActions( bindings );

		ddr::MeshRenderer mesh_renderer( mouse_picking );

		Vector<ISystem*> systems;
		systems.Add( &renderer );
		systems.Add( &scene_graph );
		//systems.Add( &swarm_system );
		//systems.Add( &trench_system );
		systems.Add( &mouse_picking );
		//systems.Add( s_shipSystem );
		systems.Add( &terrain_system );
		systems.Add( &particle_system );

		Vector<IRenderer*> renderers;
		renderers.Add( &mouse_picking );
		renderers.Add( &terrain_system );
		renderers.Add( &particle_system );
		renderers.Add( &mesh_renderer );

		BindKeys( *s_input );

		s_frameTimer = new FrameTimer();
		DebugConsole console( scriptEngine );

		Vector<IDebugPanel*> debug_views;
		debug_views.Add( s_frameTimer );
		debug_views.Add( &console );
		debug_views.Add( &renderer );
		debug_views.Add( &mouse_picking );
		debug_views.Add( s_freeCam );
		//debug_views.Add( s_shipSystem );
		debug_views.Add( &terrain_system );
		debug_views.Add( &shaky_cam );
		debug_views.Add( &particle_system );
		debug_views.Add( &mesh_renderer );

		s_fsm = new FSM();
		s_fsm->AddState( INITIALIZED );

		FSMState& updateTimerState = s_fsm->AddState( UPDATE_TIMER );
		auto onUpdateTimer = [&]()
		{
			s_frameTimer->SetMaxFPS( s_maxFPS );
			s_frameTimer->Update();

			float delta_t = s_frameTimer->Delta();
			UpdateInput( *s_input, bindings, delta_t );
			s_debugUI->Update( delta_t );
		};
		updateTimerState.SetOnEnter( onUpdateTimer );

		FSMState& preupdateState = s_fsm->AddState( PREUPDATE );
		auto onPreupdate = [&]()
		{
			float delta_t = s_frameTimer->Delta();
			entity_manager.Update( delta_t );
			PreUpdateSystems( jobSystem, entity_manager, systems, delta_t );
		};
		preupdateState.SetOnEnter( onPreupdate );

		FSMState& updateState = s_fsm->AddState( UPDATE );
		auto onUpdate = [&]()
		{
			float delta_t = s_frameTimer->Delta();
			camera.SetAspectRatio( s_window->GetWidth(), s_window->GetHeight() );

			UpdateFreeCam( *s_freeCam, shaky_cam, *s_input, delta_t );
			UpdateSystems( jobSystem, entity_manager, systems, delta_t );
		};
		updateState.SetOnEnter( onUpdate );

		FSMState& renderState = s_fsm->AddState( RENDER );
		auto onRender = [&]()
		{
			float delta_t = s_frameTimer->Delta();
			DrawDebugUI( debug_views );
			Render( renderer, renderers, entity_manager, shaky_cam, uniforms, delta_t );
		};
		renderState.SetOnEnter( onRender );

		FSMState& endFrameState = s_fsm->AddState( RENDER_END_FRAME );
		auto onEndFrame = [&]()
		{
			renderer.EndRender( camera );

			ImGui::Render();

			s_window->Swap();
		};
		endFrameState.SetOnEnter( onEndFrame );

		FSMState& postRenderState = s_fsm->AddState( POSTRENDER );
		auto onPostRender = [&]()
		{
			PostRenderSystems( jobSystem, entity_manager, systems, s_frameTimer->Delta() );

			s_frameTimer->DelayFrame();
		};
		postRenderState.SetOnEnter( onPostRender );

		FSMState& openAssertState = s_fsm->AddState( OPEN_ASSERT );

		FSMState& assertDialogState = s_fsm->AddState( ASSERT_DIALOG );
		auto onAssertDialog = [&]()
		{
			DrawAssertDialog( s_window->GetSize(), s_assert );
		};
		assertDialogState.SetOnEnter( onAssertDialog );

		s_fsm->AddTransition( INITIALIZED, UPDATE_TIMER );
		s_fsm->AddTransition( UPDATE_TIMER, PREUPDATE );
		s_fsm->AddTransition( PREUPDATE, UPDATE );
		s_fsm->AddTransition( UPDATE, RENDER );
		s_fsm->AddTransition( RENDER, RENDER_END_FRAME );
		s_fsm->AddTransition( RENDER_END_FRAME, POSTRENDER );
		s_fsm->AddTransition( POSTRENDER, UPDATE_TIMER );

		s_fsm->AddTransition( INITIALIZED, OPEN_ASSERT );
		s_fsm->AddTransition( UPDATE_TIMER, OPEN_ASSERT );
		s_fsm->AddTransition( PREUPDATE, OPEN_ASSERT );
		s_fsm->AddTransition( UPDATE, OPEN_ASSERT );
		s_fsm->AddTransition( RENDER, OPEN_ASSERT );
		s_fsm->AddTransition( POSTRENDER, OPEN_ASSERT );
		s_fsm->AddTransition( RENDER_END_FRAME, OPEN_ASSERT );

		s_fsm->AddTransition( OPEN_ASSERT, UPDATE_TIMER );
		s_fsm->AddTransition( UPDATE_TIMER, ASSERT_DIALOG );
		s_fsm->AddTransition( ASSERT_DIALOG, RENDER_END_FRAME );
		s_fsm->AddTransition( RENDER_END_FRAME, UPDATE_TIMER );

		s_fsm->Initialize( INITIALIZED );

		InitializeSystems( jobSystem, entity_manager, systems );
		InitializeRenderers( renderer, renderers );

		// everything's set up, so we can start using ImGui - asserts before this will be handled by the default console
		pempek::assert::implementation::setAssertHandler( OnAssert ); 
		::ShowWindow( GetConsoleWindow(), SW_HIDE );

		while( !s_window->ShouldClose() )
		{
			DD_PROFILE_SCOPED( Frame );

			CheckAssert();
			
			s_fsm->TransitionTo( UPDATE_TIMER );
			s_fsm->TransitionTo( PREUPDATE );
			s_fsm->TransitionTo( UPDATE );
			s_fsm->TransitionTo( RENDER );
			s_fsm->TransitionTo( RENDER_END_FRAME );
			s_fsm->TransitionTo( POSTRENDER );
		}

		ShutdownSystems( entity_manager, systems );
		systems.Clear();

		entity_manager.DestroyAll();

		renderer.Shutdown();
	}

	delete s_freeCam;
	delete s_shipSystem;

	s_window->Close();
	delete s_window;

	DD_PROFILE_DEINIT();

	return 0;
}

#ifdef _TEST

int TestMain( int argc, char* argv[] )
{
	int iError = tests::RunTests( argc, argv );

	if( iError != 0 )
		DD_ASSERT( false, "Tests failed!" );
	else
		printf( "Tests passed!" );

	return iError;
}

#endif

//
// ENTRY POINT
//
int main( int argc, char* argv[] )
{
	TypeInfo::RegisterDefaultTypes();

	CommandLine cmdLine( argv, argc );
	if( cmdLine.Exists( "noassert" ) )
	{
		pempek::assert::implementation::ignoreAllAsserts( true );
	}

	if( cmdLine.Exists( "dataroot" ) )
	{
		File::SetDataRoot( cmdLine.GetValue( "dataroot" ).c_str() );
	}
	else
	{
		File::SetDataRoot( "../../../data" );
	}

	REGISTER_TYPE( CommandLine );

	AngelScriptEngine scriptEngine;
	REGISTER_TYPE( AngelScriptEngine );

	EntityManager entity_manager;
	REGISTER_TYPE( EntityManager );
	s_entityManager = &entity_manager;

	TypeInfo::SetScriptEngine( &scriptEngine );

	RegisterGameTypes( entity_manager, scriptEngine );
	RegisterGlobalScriptFunctions( scriptEngine );

#ifdef _TEST
	return TestMain( argc, argv );
#else
	return GameMain( entity_manager, scriptEngine );
#endif
}
