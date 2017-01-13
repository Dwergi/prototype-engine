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
#include "Input.h"
#include "InputBindings.h"
#include "JobSystem.h"
#include "MeshComponent.h"
#include "Message.h"
#include "OctreeComponent.h"
#include "Random.h"
#include "Recorder.h"
#include "Renderer.h"
#include "ScopedTimer.h"
#include "StringBinding.h"
#include "SwarmAgentComponent.h"
#include "SwarmSystem.h"
#include "TerrainSystem.h"
#include "Timer.h"
#include "TransformComponent.h"
#include "Window.h"

#include "DebugConsole.h"

#include "imgui/imgui.h"

#include "Remotery/lib/Remotery.h"

#include "glm/gtc/matrix_transform.hpp"

#include "GL/gl3w.h"

#include "SFML/Network/UdpSocket.hpp"
//---------------------------------------------------------------------------

using namespace dd;

extern bool s_drawFPS;
bool s_drawFPS = true;

extern float s_maxFPS;
float s_maxFPS = 60.0f;

const int SLIDING_WINDOW_SIZE = 60;
float s_frameTimes[SLIDING_WINDOW_SIZE];
int s_currentFrame = 0;

bool s_drawConsole = false;

bool s_drawCameraDebug = true;

std::unique_ptr<Window> s_window;
std::unique_ptr<Input> s_input;

#define REGISTER_GLOBAL_VARIABLE( engine, var ) engine.RegisterGlobalVariable<decltype(var), var>( #var )

void DrawFPS( float delta_t )
{
	ImGui::SetNextWindowPos( ImVec2( 2, 2 ) );
	if( !ImGui::Begin( "FPS", &s_drawFPS, ImVec2( 0, 0 ), 0.4f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings ) )
	{
		ImGui::End();
		return;
	}

	s_frameTimes[s_currentFrame] = delta_t;
	++s_currentFrame;

	if( s_currentFrame >= SLIDING_WINDOW_SIZE )
		s_currentFrame = 0;

	float ms = delta_t * 1000.f;

	float total_time = 0;
	for( float f : s_frameTimes )
	{
		total_time += f;
	}

	float sliding_delta = (total_time / SLIDING_WINDOW_SIZE) * 1000.f;

	ImGui::Text( "Frame time: %.1f", ms );
	ImGui::Text( "Sliding: %.1f", sliding_delta );
	ImGui::End();
}

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

	REGISTER_GLOBAL_VARIABLE( engine, s_drawFPS );
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

	Services::RegisterComponent<TransformComponent>();
	Services::RegisterComponent<OctreeComponent>();
	Services::RegisterComponent<SwarmAgentComponent>();
	Services::RegisterComponent<MeshComponent>();

	manager.RegisterComponent<TransformComponent>();
	manager.RegisterComponent<OctreeComponent>();
	manager.RegisterComponent<SwarmAgentComponent>();
	manager.RegisterComponent<MeshComponent>();

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

class FrameTimer
{
public:

	FrameTimer()
	{
		m_targetDelta = 1.0f / s_maxFPS;
		m_lastFrameTime = 0.0f;
		m_currentFrameTime = -m_targetDelta;
		m_delta = m_targetDelta;

		// fill history with standard deltas
		for( int i = 0; i < SLIDING_WINDOW_SIZE; ++i )
		{
			s_frameTimes[i] = m_targetDelta;
		}

		m_timer.Start();
	}

	void Update()
	{
		m_targetDelta = 1.0f / s_maxFPS;
		m_lastFrameTime = m_currentFrameTime;
		m_currentFrameTime = m_timer.Time();
		m_delta = m_currentFrameTime - m_lastFrameTime;
	}

	float Delta() const
	{
		return m_delta;
	}

	void DelayFrame()
	{
		DD_PROFILE_START( FrameTimer_DelayFrame );

		float now = m_timer.Time();
		while( now - m_lastFrameTime < m_targetDelta )
		{
			::Sleep( 0 );

			now = m_timer.Time();
		}

		DD_PROFILE_END();
	}

private:

	Timer m_timer;
	float m_targetDelta;
	float m_lastFrameTime;
	float m_currentFrameTime;
	float m_delta;

};

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

		Vector<ISystem*> systems;
		systems.Add( &swarm_system );

		s_window.reset( new Window( 1280, 720, "DD" ) );
		s_input.reset( new Input( *s_window ) );
		s_input->CaptureMouse( !s_drawConsole );

		DebugUI debugUI( *s_window, *s_input );

		DebugConsole console;

		Renderer renderer;
		renderer.Initialize( *s_window, entity_manager );

		TerrainSystem terrain( renderer.GetCamera() );
		terrain.Initialize( entity_manager );

		InputBindings bindings;
		bindings.RegisterHandler( InputAction::CONSOLE, &ToggleConsole );

		FreeCameraController free_cam( renderer.GetCamera() );
		free_cam.BindActions( bindings );

		BindKeys( *s_input );

		FrameTimer frameTimer;

		while( !s_window->ShouldClose() )
		{
			DD_PROFILE_START( Frame );

			frameTimer.Update();
			float delta_t = frameTimer.Delta();

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

			for( ISystem* system : systems )
			{
				jobsystem.Schedule( [&entity_manager, system, delta_t]() { system->Update( entity_manager, delta_t ); }, "System" );
			}

			jobsystem.WaitForCategory( "System" );

			renderer.Render( delta_t );
			renderer.DrawDebugUI();

			if( s_drawConsole )
				console.Draw( "Console", s_drawConsole );

			if( s_drawFPS )
				DrawFPS( delta_t );

			ImGui::Render();

			s_window->Swap();

			frameTimer.DelayFrame();

			DD_PROFILE_END();

			DD_PROFILE_LOG( "End Frame" );
		}

		s_window->Close();

		s_input.reset();
		s_window.reset();
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
