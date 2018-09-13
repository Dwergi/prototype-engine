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

#include "BoundsComponent.h"
#include "BoundsRenderer.h"
#include "DebugUI.h"
#include "DDAssertHelpers.h"
#include "DoubleBuffer.h"
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
#include "LightRenderer.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MeshRenderer.h"
#include "Message.h"
#include "MousePicking.h"
#include "OctreeComponent.h"
#include "OpenGL.h"
#include "Random.h"
#include "RayRenderer.h"
#include "Recorder.h"
#include "WorldRenderer.h"
#include "FrameBuffer.h"
#include "ParticleSystem.h"
#include "ParticleSystemComponent.h"
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
#include "World.h"

#include "DebugConsole.h"

#include "imgui/imgui.h"




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
ddc::World* s_world = nullptr;
AngelScriptEngine* s_scriptEngine = nullptr;
FrameTimer* s_frameTimer = nullptr;
ShipSystem* s_shipSystem = nullptr;
FSM* s_fsm = nullptr;
InputBindings* s_inputBindings = nullptr;
DebugConsole* s_debugConsole = nullptr;

ddr::WorldRenderer* s_renderer = nullptr;

Assert s_assert;

std::thread::id s_mainThread;

#define REGISTER_GLOBAL_VARIABLE( engine, var ) engine.RegisterGlobalVariable<decltype(var), var>( #var )

/*TransformComponent* GetTransformComponent( EntityHandle entity )
{
	return entity.Get<TransformComponent>().Write();
}

ddc::Entity GetEntityHandle( uint id )
{
	return s_world->GetEntityHandle( id );
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

	DD_REGISTER_POD( glm::vec3 );
	TypeInfo* vec3Type = TypeInfo::AccessType<glm::vec3>();
	vec3Type->RegisterScriptType<glm::vec3, true>();
	vec3Type->RegisterMember<glm::vec3, float, &glm::vec3::x>( "x" );
	vec3Type->RegisterMember<glm::vec3, float, &glm::vec3::y>( "y" );
	vec3Type->RegisterMember<glm::vec3, float, &glm::vec3::z>( "z" );

	DD_REGISTER_POD( glm::vec4 );
	TypeInfo* vec4Type = TypeInfo::AccessType<glm::vec4>();
	vec4Type->RegisterScriptType<glm::vec4, true>();
	vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::x>( "x" );
	vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::y>( "y" );
	vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::z>( "z" );
	vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::z>( "w" );

	DD_REGISTER_POD( glm::mat4 );

	DD_REGISTER_TYPE( EntityHandle );
	DD_REGISTER_TYPE( IComponent );
	DD_REGISTER_TYPE( Message );
	DD_REGISTER_TYPE( ddr::MeshHandle );

	TypeInfo::RegisterComponent<TransformComponent>( "TransformComponent" );
	TypeInfo::RegisterComponent<OctreeComponent>( "OctreeComponent" );
	TypeInfo::RegisterComponent<SwarmAgentComponent>( "SwarmAgentComponent" );
	TypeInfo::RegisterComponent<MeshComponent>( "MeshComponent" );
	TypeInfo::RegisterComponent<ShipComponent>( "ShipComponent" );
	TypeInfo::RegisterComponent<ScriptComponent>( "ScriptComponent" );
	TypeInfo::RegisterComponent<LightComponent>( "LightComponent" );
	TypeInfo::RegisterComponent<TerrainChunkComponent>( "TerrainChunkComponent" );
}
*/

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

void UpdateInput( Input& input, InputBindings& bindings, float delta_t )
{
	input.Update( delta_t );

	// update input
	Array<InputEvent, 64> events;
	input.GetKeyEvents( events );
	bindings.Dispatch( events );
}

void CheckAssert()
{
	if( s_assert.Open )
	{
		static dd::String256 s_message;
		s_message = s_assert.Info;
		s_message += s_assert.Message;

		printf( s_message.c_str() );

		s_input->CaptureMouse( false );

		do
		{
			s_frameTimer->Update();

			float delta_t = s_frameTimer->AppDelta();
			UpdateInput( *s_input, *s_inputBindings, delta_t );
			s_debugUI->Update( delta_t );

			DrawAssertDialog( s_window->GetSize(), s_assert );

			ImGui::Render();

			s_window->Swap();
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

void DrawDebugUI( const Vector<IDebugPanel*>& views, const ddc::World& world )
{
	if( s_showDebugUI )
	{
		if( ImGui::BeginMainMenuBar() )
		{
			if( ImGui::BeginMenu( "File" ) )
			{
				if( ImGui::MenuItem( "Exit" ) )
				{
					s_window->SetToClose();
				}

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
		{
			debug_view->DrawDebugPanel( world );
		}
	}

	s_frameTimer->DrawFPSCounter();
}

ddc::Entity CreateMeshEntity( ddc::World& world, ddr::MeshHandle mesh_h, glm::vec4 colour, const glm::mat4& transform )
{
	ddc::Entity entity = world.CreateEntity<dd::TransformComponent, dd::MeshComponent, dd::BoundsComponent>();

	dd::TransformComponent* transform_cmp = world.Access<dd::TransformComponent>( entity );
	transform_cmp->Local = transform;

	dd::MeshComponent* mesh_cmp = world.Access<dd::MeshComponent>( entity );
	mesh_cmp->Mesh = mesh_h;
	mesh_cmp->Colour = colour;

	dd::BoundsComponent* bounds_cmp = world.Access<dd::BoundsComponent>( entity );
	bounds_cmp->LocalBox = ddr::Mesh::Get( mesh_h )->GetBoundBox();

	world.AddTag( entity, ddc::Tag::Visible );

	return entity;
}

void CreateUnitCube()
{
	ddr::MeshHandle unitCube = ddr::Mesh::Find( "unitcube" );
	if( !unitCube.IsValid() )
	{
		unitCube = ddr::Mesh::Create( "unitcube" );

		ddr::Mesh* mesh = ddr::Mesh::Get( unitCube );
		DD_ASSERT( mesh != nullptr );

		ddr::ShaderHandle shader_h = ddr::ShaderProgram::Load( "standard" );
		ddr::ShaderProgram* shader = ddr::ShaderProgram::Get( shader_h );
		DD_ASSERT( shader != nullptr );

		ddr::MaterialHandle material_h = ddr::Material::Create( "standard" );
		ddr::Material* material = ddr::Material::Get( material_h );
		DD_ASSERT( material != nullptr );

		material->SetShader( shader_h );
		mesh->SetMaterial( material_h );

		shader->Use( true );

		mesh->MakeUnitCube();

		shader->Use( false );
	}
}

int GameMain()
{
	DD_PROFILE_INIT();
	DD_PROFILE_THREAD_NAME( "Main" );

	unsigned int threads = std::thread::hardware_concurrency();
	JobSystem jobsystem( threads - 1 );

	s_mainThread = std::this_thread::get_id();

	{
		s_window = new Window( glm::ivec2( 1280, 720 ), "DD" );
		s_input = new Input( *s_window );

		s_inputBindings = new InputBindings();
		s_inputBindings->RegisterHandler( InputAction::TOGGLE_FREECAM, &ToggleFreeCam );
		s_inputBindings->RegisterHandler( InputAction::TOGGLE_DEBUG_UI, &ToggleDebugUI );
		s_inputBindings->RegisterHandler( InputAction::EXIT, &Exit );
		s_inputBindings->RegisterHandler( InputAction::BREAK, &TriggerAssert );

		BindKeys( *s_input );

		s_debugUI = new DebugUI( *s_window, *s_input );

		SceneGraphSystem* scene_graph = new SceneGraphSystem();

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
		//s_shipSystem->CreateShip( *s_world );

		TerrainSystem* terrain_system = new TerrainSystem( jobsystem );
		terrain_system->DependsOn( *scene_graph );

		ddr::ParticleSystem* particle_system = new ddr::ParticleSystem();
		particle_system->BindActions( *s_inputBindings );
		particle_system->DependsOn( *scene_graph );

		ddr::ParticleSystemRenderer* particle_renderer = new ddr::ParticleSystemRenderer();

		s_world = new ddc::World( jobsystem );

		s_world->RegisterSystem( *terrain_system );
		s_world->RegisterSystem( *scene_graph );
		s_world->RegisterSystem( *particle_system );

		s_renderer = new ddr::WorldRenderer( *s_window );

		ddr::MeshRenderer* mesh_renderer = new ddr::MeshRenderer( *mouse_picking );

		ddr::LightRenderer* light_renderer = new ddr::LightRenderer();

		ddr::BoundsRenderer* bounds_renderer = new ddr::BoundsRenderer();

		ddr::RayRenderer* ray_renderer = new ddr::RayRenderer();

		s_renderer->Register( *mouse_picking );
		s_renderer->Register( *terrain_system );
		s_renderer->Register( *light_renderer );
		s_renderer->Register( *particle_renderer );
		s_renderer->Register( *mesh_renderer );
		s_renderer->Register( *bounds_renderer );
		s_renderer->Register( *ray_renderer );

		s_frameTimer = new FrameTimer();
		s_frameTimer->SetMaxFPS( s_maxFPS );

		//s_debugConsole = new DebugConsole( *s_scriptEngine );

		Vector<IDebugPanel*> debug_views;
		debug_views.Add( s_frameTimer );
		debug_views.Add( s_renderer );
		debug_views.Add( s_freeCamera );
		debug_views.Add( mouse_picking );
		debug_views.Add( s_shakyCamera );
		debug_views.Add( particle_system );
		debug_views.Add( terrain_system );
		debug_views.Add( mesh_renderer );
		debug_views.Add( bounds_renderer );

		//debug_views.Add( s_debugConsole );
		//debug_views.Add( s_shipSystem );

		s_world->Initialize();

		s_renderer->InitializeRenderers( *s_world );

		CreateUnitCube();

		// dir light
		{
			ddc::Entity entity = s_world->CreateEntity<ddr::LightComponent, dd::TransformComponent>();
			ddr::LightComponent* light = s_world->Access<ddr::LightComponent>( entity );

			light->IsDirectional = true;
			light->Colour = glm::vec3( 1, 1, 1 );
			light->Intensity = 0.5;

			dd::TransformComponent* transform = s_world->Access<dd::TransformComponent>( entity );
			glm::vec3 direction( 0.5, 0.4, -0.3 );
			transform->Local[ 3 ].xyz = direction;
		}
		
		// particle system
		{
			ddc::Entity entity = s_world->CreateEntity<dd::ParticleSystemComponent, dd::TransformComponent, dd::BoundsComponent>();
			s_world->AddTag( entity, ddc::Tag::Visible );

			dd::TransformComponent* transform = s_world->Access<dd::TransformComponent>( entity );
			transform->SetLocalPosition( glm::vec3( 10, 60, 10 ) );

			dd::BoundsComponent* bounds = s_world->Access<dd::BoundsComponent>( entity );
			bounds->LocalBox = dd::AABB( glm::vec3( -0.5 ), glm::vec3( 0.5 ) );

			dd::ParticleSystemComponent* particle = s_world->Access<dd::ParticleSystemComponent>( entity );
			particle->m_age = 0;
			particle->m_lifetime = 1000;
		}

		// axes
		{
			ddr::MeshHandle unitCube = ddr::Mesh::Find( "unitcube" );

			CreateMeshEntity( *s_world, unitCube, glm::vec4( 1, 0, 0, 1 ), glm::translate( glm::vec3( -50.0f, 0.0f, 0.0f ) ) * glm::scale( glm::vec3( 100, 0.05f, 0.05f ) ) );
			CreateMeshEntity( *s_world, unitCube, glm::vec4( 0, 1, 0, 1 ), glm::translate( glm::vec3( 0.0f, -50.0f, 0.0f ) ) * glm::scale( glm::vec3( 0.05f, 100, 0.05f ) ) );
			CreateMeshEntity( *s_world, unitCube, glm::vec4( 0, 0, 1, 1 ), glm::translate( glm::vec3( 0.0f, 0.0f, -50.0f ) ) * glm::scale( glm::vec3( 0.05f, 0.05f, 100 ) ) );
		}

		// everything's set up, so we can start using ImGui - asserts before this will be handled by the default console
		pempek::assert::implementation::setAssertHandler( OnAssert ); 
		::ShowWindow( GetConsoleWindow(), SW_HIDE );

		while( !s_window->ShouldClose() )
		{
			DD_PROFILE_SCOPED( Frame );

			CheckAssert();

			s_frameTimer->SetMaxFPS( s_maxFPS );
			s_frameTimer->Update();

			float delta_t = s_frameTimer->GameDelta();

			UpdateInput( *s_input, *s_inputBindings, delta_t );
			s_debugUI->Update( delta_t );

			s_world->Update( delta_t );

			s_fpsCamera->SetAspectRatio( s_window->GetWidth(), s_window->GetHeight() );

			UpdateFreeCam( *s_freeCamera, *s_shakyCamera, *s_input, delta_t );
		
			DrawDebugUI( debug_views, *s_world );
			s_renderer->Render( *s_world, *s_shakyCamera );

			ImGui::Render();

			s_window->Swap();
			
			s_frameTimer->DelayFrame();
		}

		s_renderer->ShutdownRenderer();
		delete s_renderer;

		s_world->Shutdown();

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

	/*TypeInfo::RegisterDefaultTypes();
	DD_REGISTER_TYPE( CommandLine );

	s_scriptEngine = new AngelScriptEngine();
	DD_REGISTER_TYPE( AngelScriptEngine );

	TypeInfo::SetScriptEngine( s_scriptEngine );

	s_world = new ddc::World();
	DD_REGISTER_TYPE( ddc::World );

	RegisterGameTypes( *s_world, *s_scriptEngine );
	RegisterGlobalScriptFunctions( *s_scriptEngine ); */

#ifdef _TEST
	return TestMain( argc, argv );
#else
	return GameMain();
#endif
}
