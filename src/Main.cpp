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
#include "jobsystem.h"
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
#include "ParticleSystem.h"
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

Window* s_window = nullptr;
Input* s_input = nullptr;

ShakyCamera* s_shakyCamera = nullptr;
FPSCamera* s_fpsCamera = nullptr;
FreeCameraController* s_freeCamera = nullptr;
EntityManager* s_entityManager = nullptr;
AngelScriptEngine* s_scriptEngine = nullptr;
FrameTimer* s_frameTimer = nullptr;
ShipSystem* s_shipSystem = nullptr;
FSM* s_fsm = nullptr;
TerrainSystem* s_terrainSystem = nullptr;
InputBindings* s_inputBindings = nullptr;
DebugConsole* s_debugConsole = nullptr;

ddr::Renderer* s_renderer = nullptr;

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
		s_freeCamera->Enable( !s_freeCamera->IsEnabled() );
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
	input.BindKey( 'E', InputAction::START_PARTICLE );
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

void UpdateInput( Input& input, InputBindings& bindings, float delta_t )
{
	input.Update( delta_t );

	// update input
	Array<InputEvent, 64> events;
	input.GetKeyEvents( events );
	bindings.Dispatch( events );
}

int GameMain()
{
	DD_PROFILE_INIT();
	DD_PROFILE_THREAD_NAME( "Main" );

	JobSystem jobsystem( 7u );

	s_mainThread = std::this_thread::get_id();

	{
		s_window = new Window( glm::ivec2( 1280, 720 ), "DD" );
		s_input = new Input( *s_window );

		s_inputBindings = new InputBindings();
		s_inputBindings->RegisterHandler( InputAction::TOGGLE_FREECAM, &ToggleFreeCam );
		s_inputBindings->RegisterHandler( InputAction::TOGGLE_DEBUG_UI, &ToggleDebugUI );
		s_inputBindings->RegisterHandler( InputAction::EXIT, &Exit );
		s_inputBindings->RegisterHandler( InputAction::BREAK, &TriggerAssert );

		s_debugUI = new DebugUI( *s_window, *s_input );

		s_renderer = new ddr::Renderer( *s_window );

		SceneGraphSystem scene_graph;

		//SwarmSystem swarm_system;

		//TrenchSystem trench_system( *s_shakyCam  );
		//trench_system.CreateRenderResources();

		s_fpsCamera = new FPSCamera( *s_window );
		s_fpsCamera->SetPosition( glm::vec3( 0, 10, 0 ) );
		s_fpsCamera->SetDirection( glm::vec3( 0, 0, 1 ) );

		s_shakyCamera = new ShakyCamera( *s_fpsCamera, *s_inputBindings );
		
		s_freeCamera = new FreeCameraController( *s_fpsCamera );
		s_freeCamera->BindActions( *s_inputBindings );

		MousePicking* mouse_picking = new MousePicking( *s_window, *s_input );
		mouse_picking->BindActions( *s_inputBindings );

		//ShipSystem ship_system( *s_shakyCam  );
		//s_shipSystem = &ship_system;
		//s_shipSystem->BindActions( bindings );
		//s_shipSystem->CreateShip( *s_entityManager );

		s_terrainSystem = new TerrainSystem( jobsystem );

		ddr::ParticleSystem* particle_system = new ddr::ParticleSystem();
		particle_system->BindActions( *s_inputBindings );

		ddr::MeshRenderer* mesh_renderer = new ddr::MeshRenderer( *mouse_picking );

		Vector<ISystem*> systems;
		systems.Add( s_renderer );
		systems.Add( &scene_graph );
		//systems.Add( &swarm_system );
		//systems.Add( &trench_system );
		systems.Add( mouse_picking );
		//systems.Add( s_shipSystem );
		systems.Add( s_terrainSystem );
		
		s_renderer->Register( *mouse_picking );
		s_renderer->Register( *mesh_renderer );
		s_renderer->Register( *s_terrainSystem );
		s_renderer->Register( *particle_system );

		BindKeys( *s_input );

		s_frameTimer = new FrameTimer();

		s_debugConsole = new DebugConsole( *s_scriptEngine );

		Vector<IDebugPanel*> debug_views;
		debug_views.Add( s_frameTimer );
		debug_views.Add( s_debugConsole );
		debug_views.Add( s_renderer );
		debug_views.Add( mouse_picking );
		debug_views.Add( s_freeCamera );
		//debug_views.Add( s_shipSystem );
		debug_views.Add( s_terrainSystem );
		debug_views.Add( s_shakyCamera );
		debug_views.Add( particle_system );
		debug_views.Add( mesh_renderer );

		s_fsm = new FSM();
		s_fsm->AddState( INITIALIZED );

		s_fsm->AddState( UPDATE_TIMER );
		auto onUpdateTimer = [&]()
		{
			s_frameTimer->SetMaxFPS( s_maxFPS );
			s_frameTimer->Update();

			float delta_t = s_frameTimer->Delta();
			UpdateInput( *s_input, *s_inputBindings, delta_t );
			s_debugUI->Update( delta_t );
		};
		s_fsm->SetOnEnter( UPDATE_TIMER, onUpdateTimer );

		s_fsm->AddState( PREUPDATE );
		auto onPreupdate = [&]()
		{
			float delta_t = s_frameTimer->Delta();
			s_entityManager->Update( delta_t );
			PreUpdateSystems( jobsystem, *s_entityManager, systems, delta_t );
		};
		s_fsm->SetOnEnter( PREUPDATE, onPreupdate );

		s_fsm->AddState( UPDATE );
		auto onUpdate = [&]()
		{
			float delta_t = s_frameTimer->Delta();
			s_fpsCamera->SetAspectRatio( s_window->GetWidth(), s_window->GetHeight() );

			UpdateFreeCam( *s_freeCamera, *s_shakyCamera, *s_input, delta_t );
			UpdateSystems( jobsystem, *s_entityManager, systems, delta_t );
		};
		s_fsm->SetOnEnter( UPDATE, onUpdate );

		s_fsm->AddState( RENDER );
		auto onRender = [&]()
		{
			float delta_t = s_frameTimer->Delta();
			DrawDebugUI( debug_views );
			s_renderer->Render( *s_entityManager, *s_shakyCamera );
		};
		s_fsm->SetOnEnter( RENDER, onRender );

		s_fsm->AddState( RENDER_END_FRAME );
		auto onEndFrame = [&]()
		{
			s_renderer->EndRender( *s_shakyCamera );

			ImGui::Render();

			s_window->Swap();
		};
		s_fsm->SetOnEnter( RENDER_END_FRAME, onEndFrame );

		s_fsm->AddState( POSTRENDER );
		auto onPostRender = [&]()
		{
			PostRenderSystems( jobsystem, *s_entityManager, systems, s_frameTimer->Delta() );

			s_frameTimer->DelayFrame();
		};
		s_fsm->SetOnEnter( POSTRENDER, onPostRender );

		s_fsm->AddState( OPEN_ASSERT );

		s_fsm->AddState( ASSERT_DIALOG );
		auto onAssertDialog = [&]()
		{
			DrawAssertDialog( s_window->GetSize(), s_assert );
		};
		s_fsm->SetOnEnter( ASSERT_DIALOG, onAssertDialog );

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

		InitializeSystems( jobsystem, *s_entityManager, systems );
		s_renderer->RenderInit();

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

		ShutdownSystems( *s_entityManager, systems );
		systems.Clear();

		s_entityManager->DestroyAll();

		s_renderer->Shutdown();
		delete s_renderer;

		s_window->Close();
		delete s_window;
	}

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

	TypeInfo::RegisterDefaultTypes();
	REGISTER_TYPE( CommandLine );

	s_scriptEngine = new AngelScriptEngine();
	REGISTER_TYPE( AngelScriptEngine );

	TypeInfo::SetScriptEngine( s_scriptEngine );

	s_entityManager = new EntityManager();
	REGISTER_TYPE( EntityManager );

	RegisterGameTypes( *s_entityManager, *s_scriptEngine );
	RegisterGlobalScriptFunctions( *s_scriptEngine );

#ifdef _TEST
	return TestMain( argc, argv );
#else
	return GameMain();
#endif
}
