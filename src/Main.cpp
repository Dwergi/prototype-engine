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
#include "RenderToTexture.h"
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

FreeCameraController* s_freeCam;
ShipSystem* s_shipSystem;
Window* s_window;
EntityManager* s_entity_manager;

#define REGISTER_GLOBAL_VARIABLE( engine, var ) engine.RegisterGlobalVariable<decltype(var), var>( #var )

TransformComponent* GetTransformComponent( EntityHandle entity )
{
	return entity.Get<TransformComponent>().Write();
}

EntityHandle GetEntityHandle( uint id )
{
	EntityHandle handle( id, *s_entity_manager );
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

void InitializeRenderers( const EntityManager& entity_manager, const ICamera& camera, const Vector<IRenderer*>& systems )
{
	for( IRenderer* renderer : systems )
	{
		renderer->RenderInit( entity_manager, camera );
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

void Render( const Vector<IRenderer*>& renderers, EntityManager& entity_manager, const ICamera& camera, DebugConsole& console, FrameTimer& frame_timer, RenderToTexture& rtt )
{
	for( IRenderer* renderer : renderers )
	{
		renderer->Render( entity_manager, camera );

		if( renderer->ShouldRenderFrameBuffer() )
		{
			const ConstBuffer<byte>* frame_buffer_data = renderer->GetLastFrameBuffer();
			if( frame_buffer_data != nullptr )
			{
				Texture* rtt_texture = rtt.GetTexture();
				rtt_texture->Bind( 0 );
				rtt_texture->SetData( *frame_buffer_data, 0 );
				rtt_texture->Unbind();

				rtt.Render();
			}
		}
	}
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

		s_window = new Window( 1280, 720, "DD" );
		Input input( *s_window );

		InputBindings bindings;
		bindings.RegisterHandler( InputAction::TOGGLE_FREECAM, &ToggleFreeCam );
		bindings.RegisterHandler( InputAction::TOGGLE_DEBUG_UI, &ToggleDebugUI );
		bindings.RegisterHandler( InputAction::EXIT, &Exit );

		FPSCamera camera( *s_window );
		camera.SetPosition( glm::vec3( 0, 10, 0 ) );
		camera.SetDirection( glm::vec3( 0, 0, 1 ) );

		ShakyCamera shakyCam( camera, bindings );

		DebugUI debugUI( *s_window, input );

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

		MousePicking mouse_picking( *s_window, camera, input );
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
		renderers.Add( &renderer );

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
		debug_views.Add( &terrain_system );
		debug_views.Add( &shakyCam );

		InitializeSystems( jobSystem, entity_manager, systems );
		InitializeRenderers( entity_manager, shakyCam, renderers );

		glm::ivec2 picking_size( s_window->GetWidth() / MousePicking::DownScalingFactor, 
			s_window->GetHeight() / MousePicking::DownScalingFactor );

		Texture output_texture;
		output_texture.Create( picking_size, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );

		RenderToTexture output_rtt( *s_window );
		output_rtt.Create( output_texture, false );
		output_rtt.PreRender();

		while( !s_window->ShouldClose() )
		{
			DD_PROFILE_SCOPED( Frame );

			// frame timer
			frame_timer.SetMaxFPS( s_maxFPS );
			frame_timer.Update();
			float delta_t = frame_timer.Delta();

			// entity manager
			entity_manager.Update( delta_t );

			// systems pre-update
			PreUpdateSystems( jobSystem, entity_manager, systems, delta_t );

			// input
			UpdateInput( input, bindings, delta_t );

			// debug UI
			debugUI.Update( delta_t );

			// camera
			UpdateFreeCam( *s_freeCam, shakyCam, input, delta_t );

			// systems update
			UpdateSystems( jobSystem, entity_manager, systems, delta_t );

			// debug UI
			DrawDebugUI( debug_views );

			// render
			Render( renderers, entity_manager, shakyCam, console, frame_timer, output_rtt );

			ImGui::Render();

			s_window->Swap();

			// systems post-render
			PostRenderSystems( jobSystem, entity_manager, systems, delta_t );

			camera.SetClean();

			// wait for frame delta
			frame_timer.DelayFrame();

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

	EntityManager entity_manager;
	REGISTER_TYPE( EntityManager );
	s_entity_manager = &entity_manager;

	TypeInfo::SetScriptEngine( &scriptEngine );

	RegisterGameTypes( entity_manager, scriptEngine );
	RegisterGlobalScriptFunctions( scriptEngine );

#ifdef _TEST
	return TestMain( argc, argv );
#else
	return GameMain( entity_manager, scriptEngine );
#endif
}
