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
#include "EntityManager.h"
#include "File.h"
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

extern float s_maxFPS;
float s_maxFPS = 60.0f;

bool s_showDebugUI = false;
bool s_freeCamEnabled = true;

Window* s_window;

#define REGISTER_GLOBAL_VARIABLE( engine, var ) engine.RegisterGlobalVariable<decltype(var), var>( #var )

class FrameTimer : public IDebugDraw
{
public:

	FrameTimer()
	{
		m_targetDelta = 1.0f / s_maxFPS;
		m_lastFrameTime = 0.0f;
		m_currentFrameTime = -m_targetDelta;
		m_delta = m_targetDelta;
		m_deltaWithoutDelay = m_targetDelta;

		// fill history with standard deltas
		for( int i = 0; i < SLIDING_WINDOW_SIZE; ++i )
		{
			m_frameTimes[i] = m_targetDelta;
		}

		m_timer.Start();
	}

	void Update()
	{
		m_targetDelta = 1.0f / s_maxFPS;
		m_lastFrameTime = m_currentFrameTime;
		m_currentFrameTime = m_timer.Time();
		m_delta = m_currentFrameTime - m_lastFrameTime;

		// update sliding window
		m_frameTimes[ m_currentSlidingFrame ] = m_deltaWithoutDelay;
		++m_currentSlidingFrame;

		if( m_currentSlidingFrame >= SLIDING_WINDOW_SIZE )
			m_currentSlidingFrame = 0;

		float total_time = 0;
		for( float f : m_frameTimes )
		{
			total_time += f;
		}

		m_slidingDelta = (total_time / SLIDING_WINDOW_SIZE);
	}

	float Delta() const
	{
		return m_delta;
	}

	float SlidingDelta() const
	{
		return m_slidingDelta;
	}

	float DeltaWithoutDelay() const
	{
		return m_deltaWithoutDelay;
	}

	void DelayFrame()
	{
		DD_PROFILE_SCOPED( FrameTimer_DelayFrame );

		float now = m_timer.Time();
		m_deltaWithoutDelay = now - m_lastFrameTime;

		while( now - m_lastFrameTime < m_targetDelta )
		{
			::Sleep( 0 );

			now = m_timer.Time();
		}
	}

	virtual const char* GetDebugTitle() const override
	{
		return "FPS";
	}

protected:

	virtual void DrawDebugInternal() override
	{
		ImGui::SetWindowPos( ImVec2( 2.0f, 30.0f ), ImGuiSetCond_FirstUseEver );
		ImGui::SetWindowSize( ImVec2( 150.0f, 80.0f ), ImGuiSetCond_FirstUseEver );

		ImGui::Text( "FPS: %.1f", 1.0f / m_slidingDelta );
		ImGui::Text( "Frame Time: %.1f", m_deltaWithoutDelay * 1000.f );
		ImGui::Text( "Sliding: %.1f", m_slidingDelta * 1000.f );
	}

private:

	Timer m_timer;
	float m_targetDelta;
	float m_lastFrameTime;
	float m_currentFrameTime;
	float m_delta;
	float m_deltaWithoutDelay;
	float m_slidingDelta;

	static const int SLIDING_WINDOW_SIZE = 60;
	float m_frameTimes[ SLIDING_WINDOW_SIZE ];
	int m_currentSlidingFrame = 0;
};

TransformComponent* GetTransformComponent( EntityHandle entity )
{
	return entity.Get<TransformComponent>().Write();
}

EntityHandle GetEntityHandle( uint id )
{
	EntityManager& system = Services::Get<EntityManager>();

	EntityHandle handle( id, system );
	return handle;
}

void RegisterGlobalScriptFunctions()
{
	ScriptEngine& engine = Services::Get<ScriptEngine>();

	engine.RegisterFunction<decltype(&GetTransformComponent), &GetTransformComponent>( "GetTransformComponent" );
	engine.RegisterFunction<decltype(&GetEntityHandle), &GetEntityHandle>( "GetEntityHandle" );

	REGISTER_GLOBAL_VARIABLE( engine, s_maxFPS );
}

void RegisterGameTypes( EntityManager& manager )
{
#ifdef USE_ANGELSCRIPT
	dd::RegisterString( Services::Get<AngelScriptEngine>() );
#endif

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

	REGISTER_TYPE( TransformComponent );
	REGISTER_TYPE( OctreeComponent );
	REGISTER_TYPE( SwarmAgentComponent );
	REGISTER_TYPE( MeshComponent );
	REGISTER_TYPE( ShipComponent );

	Services::RegisterComponent<TransformComponent>();
	Services::RegisterComponent<OctreeComponent>();
	Services::RegisterComponent<SwarmAgentComponent>();
	Services::RegisterComponent<MeshComponent>();
	Services::RegisterComponent<ShipComponent>();

	manager.RegisterComponent<TransformComponent>();
	manager.RegisterComponent<OctreeComponent>();
	manager.RegisterComponent<SwarmAgentComponent>();
	manager.RegisterComponent<MeshComponent>();
	manager.RegisterComponent<ShipComponent>();

	REGISTER_TYPE( SwarmSystem );

	RegisterGlobalScriptFunctions();
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
		s_freeCamEnabled = !s_freeCamEnabled;
		s_shipEnabled = !s_shipEnabled;
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
	free_cam.Enable( s_freeCamEnabled );

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
	if( !s_showDebugUI )
		return;

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

int GameMain( EntityManager& entity_manager )
{
	DD_PROFILE_INIT();
	DD_PROFILE_THREAD_NAME( "Main" );

	//::ShowWindow( GetConsoleWindow(), SW_HIDE );

	{
		JobSystem jobsystem( 2u );
		Services::Register( jobsystem );

		SwarmSystem swarm_system;

		s_window = new Window( 1280, 720, "DD" );
		Input input( *s_window );

		DebugUI debugUI( *s_window, input );

		Renderer renderer;
		renderer.Initialize( *s_window, entity_manager );

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

		FreeCameraController free_cam( camera );
		free_cam.BindActions( bindings );
		free_cam.Enable( s_freeCamEnabled );

		ShipSystem ship_system( camera );
		ship_system.BindActions( bindings );
		ship_system.CreateShip( entity_manager );
		ship_system.Enable( s_shipEnabled );

		MousePicking mouse_picking( *s_window, camera, input );
		mouse_picking.BindActions( bindings );
		renderer.SetMousePicking( &mouse_picking );

		Vector<ISystem*> systems;
		systems.Add( &scene_graph );
		systems.Add( &swarm_system );
		systems.Add( &trench_system );
		systems.Add( &mouse_picking );
		systems.Add( &ship_system );

		BindKeys( input );

		FrameTimer frame_timer;
		DebugConsole console;

		Vector<IDebugDraw*> debug_views;
		debug_views.Add( &frame_timer );
		debug_views.Add( &console );
		debug_views.Add( &renderer );
		debug_views.Add( &mouse_picking );
		debug_views.Add( &free_cam );

		while( !s_window->ShouldClose() )
		{
			DD_PROFILE_SCOPED( Frame );

			// frame timer
			frame_timer.Update();
			float delta_t = frame_timer.Delta();

			// entity manager
			entity_manager.Update( delta_t );

			// systems pre-update
			PreUpdateSystems( jobsystem, entity_manager, systems, delta_t );

			// input
			UpdateInput( input, bindings, delta_t );

			// debug UI
			debugUI.Update( delta_t );

			// camera
			UpdateFreeCam( free_cam, input, delta_t );

			ship_system.Enable( s_shipEnabled );

			// systems update
			UpdateSystems( jobsystem, entity_manager, systems, delta_t );

			// debug UI
			DrawDebugUI( debug_views );

			// render
			Render( renderer, entity_manager, console, frame_timer, delta_t );

			// systems post-render
			PostRenderSystems( jobsystem, entity_manager, systems, delta_t );

			// wait for frame delta
			frame_timer.DelayFrame();

			DD_PROFILE_LOG( "End Frame" );
		}

		systems.Clear();
		renderer.Shutdown();
	}

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
	Services::Initialize();

	TypeInfo::RegisterDefaultTypes();

	CommandLine cmdLine( argv, argc );
	if( cmdLine.Exists( "noassert" ) )
		pempek::assert::implementation::ignoreAllAsserts( true );

	if( cmdLine.Exists( "dataroot" ) )
		dd::File::SetDataRoot( cmdLine.GetValue( "dataroot" ).c_str() );
	else
		dd::File::SetDataRoot( "../../../data" );

	REGISTER_TYPE( CommandLine );
	Services::Register( cmdLine );

	// TODO: this is bad, not compatible with Wren, and registered too early anyway
	REGISTER_TYPE( ScriptEngine );

	ScriptEngine scriptEngine;
	Services::Register( scriptEngine );

	EntityManager entity_manager;
	REGISTER_TYPE( EntityManager );
	Services::Register( entity_manager );

	RegisterGameTypes( entity_manager );

#ifdef _TEST
	return TestMain( argc, argv );
#else
	return GameMain( entity_manager );
#endif
}
