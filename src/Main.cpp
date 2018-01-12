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
#include "DoubleBuffer.h"
#include "EntityManager.h"
#include "File.h"
#include "FrameTimer.h"
#include "FreeCameraController.h"
#include "FPSCamera.h"
#include "IDebugDraw.h"
#include "Input.h"
#include "InputBindings.h"
#include "JobSystem.h"
#include "LightComponent.h"
#include "MeshComponent.h"
#include "Message.h"
#include "MousePicking.h"
#include "OctreeComponent.h"
#include "Random.h"
#include "Recorder.h"
#include "Renderer.h"
#include "FrameBuffer.h"
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
DebugUI* s_debugUI = nullptr;

Input* s_input = nullptr;
FreeCameraController* s_freeCam = nullptr;
ShipSystem* s_shipSystem = nullptr;
Window* s_window = nullptr;
EntityManager* s_entityManager = nullptr;
FrameTimer* s_frameTimer = nullptr;

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
	REGISTER_TYPE( IComponent );
	REGISTER_TYPE( Message );
	REGISTER_TYPE( JobSystem );
	REGISTER_TYPE( MeshHandle );

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

void Exit( InputAction action, InputType type )
{
	if( action == InputAction::EXIT && type == InputType::RELEASED )
	{
		s_window->SetToClose();
	}
}

struct Assert
{
	String256 Info;
	String256 Message;
	bool Open;
};

static Assert s_assert;

String256 FormatAssert( int level, const char* file, int line, const char* function, const char* expression )
{
	String256 out;
	switch( level )
	{
	case AssertLevel::Debug:
		out += "DEBUG";
		break;

	case AssertLevel::Warning:
		out += "WARNING";
		break;

	case AssertLevel::Error:
		out += "ERROR";
		break;

	case AssertLevel::Fatal:
		out += "FATAL";
		break;
	}

	char buffer[ 1024 ];
	snprintf( buffer, 1024, ": \"%s\" in %s() (%s:%d)", expression, function, file, line );

	out += buffer;

	return out;
}

AssertAction ShowAssertDialog()
{
	s_input->CaptureMouse( false );
	glm::ivec2 size = s_window->GetSize();
	ImGui::SetNextWindowPos( ImVec2( size.x / 2.0f - size.x / 6.5f, size.y / 2.0f - size.y / 6.5f ), ImGuiSetCond_FirstUseEver );
	ImGui::SetNextWindowSize( ImVec2( size.x / 3.0f, size.y / 3.0f ), ImGuiSetCond_FirstUseEver );

	AssertAction action = AssertAction::None;

	float delta_t = 0.0f;

	do
	{
		if( ImGui::Begin( "Assert", &s_assert.Open ) )
		{
			ImGui::TextWrapped( s_assert.Info.c_str() );
			ImGui::TextWrapped( s_assert.Message.c_str() );
			ImGui::NewLine();

			if( ImGui::Button( "Break" ) )
			{
				action = AssertAction::Break;
			}

			ImGui::SameLine();

			if( ImGui::Button( "Ignore" ) )
			{
				action = AssertAction::Ignore;
			}

			ImGui::SameLine();

			if( ImGui::Button( "Ignore This" ) )
			{
				action = AssertAction::IgnoreLine;
			}

			ImGui::SameLine();

			if( ImGui::Button( "Abort" ) )
			{
				action = AssertAction::Abort;
			}

			ImGui::End();
		}
		else
		{
			action = AssertAction::Ignore;
		}

		ImGui::Render();

		s_window->Swap();

		s_frameTimer->Update();
		delta_t = s_frameTimer->Delta();
		
		s_input->Update( delta_t );
		s_debugUI->Update( delta_t );
	} 
	while( action == AssertAction::None );

	return action;
}

pempek::assert::implementation::AssertAction::AssertAction OnAssert( const char* file, int line, const char* function, const char* expression,
	int level, const char* message )
{
	s_assert.Open = true;
	s_assert.Info = FormatAssert( level, file, line, function, expression );
	s_assert.Message = String256( "Message: " );
	if( message != nullptr )
	{
		s_assert.Message += message;
	}

	return (pempek::assert::implementation::AssertAction::AssertAction) ShowAssertDialog();
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

void InitializeSystems( JobSystem& jobsystem, EntityManager& entity_manager, const Vector<ISystem*>& systems )
{
	for( ISystem* system : systems )
	{
		jobsystem.Schedule( [&entity_manager, system]() { system->Initialize( entity_manager ); }, "ISystem::Initialize" );
	}

	jobsystem.WaitForCategory( "ISystem::Initialize" );
}

void InitializeRenderers( Renderer& renderer, const EntityManager& entity_manager, const ICamera& camera, const Vector<IRenderer*>& renderers )
{
	renderer.RenderInit( entity_manager, camera );

	for( IRenderer* current : renderers )
	{
		current->RenderInit( entity_manager, camera );
	}
}

void PreUpdateSystems( JobSystem& jobsystem, EntityManager& entity_manager, const Vector<ISystem*>& systems, float delta_t )
{
	for( ISystem* system : systems )
	{
		jobsystem.Schedule( [&entity_manager, system, delta_t]() { system->PreUpdate( entity_manager, delta_t ); }, "ISystem::PreUpdate" );
	}

	jobsystem.WaitForCategory( "ISystem::PreUpdate" );
}

void UpdateSystems( JobSystem& jobsystem, EntityManager& entity_manager, const Vector<ISystem*>& systems, float delta_t )
{
	for( ISystem* system : systems )
	{
		jobsystem.Schedule( [&entity_manager, system, delta_t]() { system->Update( entity_manager, delta_t ); }, "ISystem::Update" );
	}

	jobsystem.WaitForCategory( "ISystem::Update" );
}

void PostRenderSystems( JobSystem& jobsystem, EntityManager& entity_manager, const Vector<ISystem*>& systems, float delta_t )
{
	for( ISystem* system : systems )
	{
		jobsystem.Schedule( [&entity_manager, system, delta_t]() { system->PostRender( entity_manager, delta_t ); }, "ISystem::PostRender" );
	}

	jobsystem.WaitForCategory( "ISystem::PostRender" );
}

void ShutdownSystems( EntityManager& entity_manager, const Vector<ISystem*>& systems )
{
	for( ISystem* system : systems )
	{
		system->Shutdown( entity_manager );
	}
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
					debug_view->AddToMenu();
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

void Render( Renderer& renderer, const Vector<IRenderer*>& renderers, EntityManager& entity_manager, const ICamera& camera, DebugConsole& console, FrameTimer& frame_timer )
{
	renderer.BeginRender( camera );

	for( IRenderer* current : renderers )
	{
		current->Render( entity_manager, camera );

		if( current->ShouldRenderFrameBuffer() )
		{
			FrameBuffer* current_fbo = current->GetFrameBuffer();
			current_fbo->Blit();
		}
	}

	renderer.Render( entity_manager, camera );

	renderer.EndRender( camera );

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

int GameMain( EntityManager& entity_manager, AngelScriptEngine& scriptEngine )
{
	DD_PROFILE_INIT();
	DD_PROFILE_THREAD_NAME( "Main" );

	//::ShowWindow( GetConsoleWindow(), SW_HIDE );

	{
		JobSystem jobSystem( 2u );
		SwarmSystem swarm_system;

		s_window = new Window( glm::ivec2( 1280, 720 ), "DD" );
		s_input = new Input( *s_window );

		InputBindings bindings;
		bindings.RegisterHandler( InputAction::TOGGLE_FREECAM, &ToggleFreeCam );
		bindings.RegisterHandler( InputAction::TOGGLE_DEBUG_UI, &ToggleDebugUI );
		bindings.RegisterHandler( InputAction::EXIT, &Exit );

		FPSCamera camera( *s_window );
		camera.SetPosition( glm::vec3( 0, 10, 0 ) );
		camera.SetDirection( glm::vec3( 0, 0, 1 ) );

		ShakyCamera shakyCam( camera, bindings );

		s_debugUI = new DebugUI( *s_window, *s_input );

		Renderer renderer( *s_window );

		TerrainSystem terrain_system( camera, jobSystem );

		SceneGraphSystem scene_graph;

		//TrenchSystem trench_system( camera );
		//trench_system.CreateRenderResources();

		s_freeCam = new FreeCameraController( camera );
		s_freeCam->BindActions( bindings );

		s_shipSystem = new ShipSystem( camera );
		s_shipSystem->BindActions( bindings );
		s_shipSystem->CreateShip( entity_manager );

		MousePicking mouse_picking( *s_window, camera, *s_input );
		mouse_picking.BindActions( bindings );
		renderer.SetMousePicking( &mouse_picking );

		Vector<ISystem*> systems;
		systems.Add( &renderer );
		systems.Add( &scene_graph );
		systems.Add( &swarm_system );
		//systems.Add( &trench_system );
		systems.Add( &mouse_picking );
		systems.Add( s_shipSystem );
		systems.Add( &terrain_system );

		Vector<IRenderer*> renderers;
		renderers.Add( &mouse_picking );
		renderers.Add( &terrain_system );

		BindKeys( *s_input );

		s_frameTimer = new FrameTimer();
		DebugConsole console( scriptEngine );

		Vector<IDebugDraw*> debug_views;
		debug_views.Add( s_frameTimer );
		debug_views.Add( &console );
		debug_views.Add( &renderer );
		debug_views.Add( &mouse_picking );
		debug_views.Add( s_freeCam );
		debug_views.Add( s_shipSystem );
		debug_views.Add( &terrain_system );
		debug_views.Add( &shakyCam );

		InitializeSystems( jobSystem, entity_manager, systems );
		InitializeRenderers( renderer, entity_manager, shakyCam, renderers );

		glm::ivec2 picking_size( s_window->GetWidth() / MousePicking::DownScalingFactor, 
			s_window->GetHeight() / MousePicking::DownScalingFactor );

		while( !s_window->ShouldClose() )
		{
			DD_PROFILE_SCOPED( Frame );

			// frame timer
			s_frameTimer->SetMaxFPS( s_maxFPS );
			s_frameTimer->Update();
			float delta_t = s_frameTimer->Delta();

			// entity manager
			entity_manager.Update( delta_t );

			// systems pre-update
			PreUpdateSystems( jobSystem, entity_manager, systems, delta_t );

			// input
			UpdateInput( *s_input, bindings, delta_t );

			// debug UI
			s_debugUI->Update( delta_t );

			// camera
			UpdateFreeCam( *s_freeCam, shakyCam, *s_input, delta_t );

			// systems update
			UpdateSystems( jobSystem, entity_manager, systems, delta_t );

			// debug UI
			DrawDebugUI( debug_views );

			DD_ASSERT( false, "TEST" );

			// render
			Render( renderer, renderers, entity_manager, shakyCam, console, *s_frameTimer );

			// systems post-render
			PostRenderSystems( jobSystem, entity_manager, systems, delta_t );

			camera.SetClean();

			// wait for frame delta
			s_frameTimer->DelayFrame();

			DD_PROFILE_LOG( "End Frame" );
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

//
// ENTRY POINT
//
int main( int argc, char* argv[] )
{
	TypeInfo::RegisterDefaultTypes();

	pempek::assert::implementation::setAssertHandler( OnAssert );

	CommandLine cmdLine( argv, argc );
	if( cmdLine.Exists( "noassert" ) )
	{
		pempek::assert::implementation::ignoreAllAsserts( true );
	}

	if( cmdLine.Exists( "dataroot" ) )
	{
		dd::File::SetDataRoot( cmdLine.GetValue( "dataroot" ).c_str() );
	}
	else
	{
		dd::File::SetDataRoot( "../../../data" );
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
