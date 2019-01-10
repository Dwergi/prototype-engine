//
// Main.cpp - Entry point into application.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#include "PCH.h"

#include "CommandLine.h"

#ifdef _TEST

#include "Tests.h"

#endif

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "BoundsRenderer.h"
#include "BulletSystem.h"
#include "ColourComponent.h"
#include "DDAssertHelpers.h"
#include "DebugConsole.h"
#include "DebugUI.h"
#include "DoubleBuffer.h"
#include "EntityPrototype.h"
#include "EntityVisualizer.h"
#include "File.h"
#include "FPSCameraComponent.h"
#include "FrameBuffer.h"
#include "FrameTimer.h"
#include "FreeCameraController.h"
#include "GLFWInputSource.h"
#include "HitTestSystem.h"
#include "IDebugPanel.h"
#include "IInputSource.h"
#include "InputBindings.h"
#include "InputSystem.h"
#include "JobSystem.h"
#include "LightComponent.h"
#include "LightRenderer.h"
#include "LinesRenderer.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MeshRenderer.h"
#include "MeshUtils.h"
#include "MessageQueue.h"
#include "MousePicking.h"
#include "OctreeComponent.h"
#include "OpenGL.h"
#include "ParticleSystem.h"
#include "ParticleSystemComponent.h"
#include "ParticleSystemRenderer.h"
#include "PhysicsPlaneComponent.h"
#include "PhysicsSphereComponent.h"
#include "PhysicsSystem.h"
#include "PlayerComponent.h"
#include "Random.h"
#include "RayComponent.h"
#include "RayRenderer.h"
#include "Recorder.h"
#include "ScopedTimer.h"
#include "ScriptComponent.h"
#include "Services.h"
#include "ShakyCamera.h"
#include "ShipComponent.h"
#include "ShipSystem.h"
#include "StringBinding.h"
#include "SwarmAgentComponent.h"
#include "SwarmSystem.h"
#include "TerrainChunkComponent.h"
#include "TerrainRenderer.h"
#include "TerrainSystem.h"
#include "Texture.h"
#include "Timer.h"
#include "TransformComponent.h"
#include "TreeSystem.h"
#include "TrenchSystem.h"
#include "Uniforms.h"
#include "WaterRenderer.h"
#include "WaterSystem.h"
#include "Window.h"
#include "World.h"
#include "WorldRenderer.h"

#include "SFML/Network/UdpSocket.hpp"

#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>
//---------------------------------------------------------------------------

using namespace dd;

extern uint s_maxFPS;
uint s_maxFPS = 60;

SERVICE_CPP( dd::Window );
SERVICE_CPP( dd::InputSystem );
SERVICE_CPP( dd::InputBindings );
SERVICE_CPP( dd::FreeCameraController );
SERVICE_CPP( ddc::World );
SERVICE_CPP( dd::AngelScriptEngine );
SERVICE_CPP( dd::FrameTimer );
SERVICE_CPP( dd::ShipSystem );
SERVICE_CPP( dd::DebugConsole );
SERVICE_CPP( ddr::WorldRenderer );
SERVICE_CPP( dd::DebugUI );
SERVICE_CPP( dd::ShakyCamera );
SERVICE_CPP( dd::FPSCameraComponent );

dd::Service<dd::Window> s_window;
dd::Service<dd::InputSystem> s_input;
dd::Service<dd::InputBindings> s_inputBindings;
dd::Service<dd::DebugUI> s_debugUI;
dd::Service<dd::FPSCameraComponent> s_player;
dd::Service<ddc::World> s_world;
dd::Service<dd::FreeCameraController> s_freeCamera;
dd::Service<ddr::WorldRenderer> s_renderer;
dd::Service<dd::FrameTimer> s_frameTimer;
dd::Service<dd::ShipSystem> s_shipSystem;
dd::Service<dd::ShakyCamera> s_shakyCamera;

std::thread::id s_mainThread;

void ToggleConsole( InputAction action, InputType type )
{
	if( action == InputAction::TOGGLE_CONSOLE && type == InputType::RELEASED )
	{
		s_debugUI->EnableDraw( true );
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
		if( s_input->Source().GetMode() == InputMode::DEBUG )
		{
			s_input->Source().SetMode( InputMode::GAME );
		}
		else
		{
			s_input->Source().SetMode( InputMode::DEBUG );
		}

		s_debugUI->EnableDraw( !s_debugUI->Draw() );
	}
}

void PauseGame( InputAction action, InputType type )
{
	if( action == InputAction::PAUSE && type == InputType::RELEASED )
	{
		s_frameTimer->SetPaused( !s_frameTimer->IsPaused() );
	}
}

void Exit( InputAction action, InputType type )
{
	if( action == InputAction::EXIT && type == InputType::RELEASED )
	{
		s_window->SetToClose();
	}
}

void SetCameraPos( InputAction action, InputType type )
{
	if( type != InputType::RELEASED )
		return;

	s_player->SetRotation( 0, 0 );

	const glm::vec3 cube_pos( 10.5, 60.5, 10 );

	switch( action )
	{
	case InputAction::CAMERA_POS_1:
		s_player->SetPosition( cube_pos - glm::vec3( 0, 0, 0.5 ) );
		break;

	case InputAction::CAMERA_POS_2:
		s_player->SetPosition( cube_pos - glm::vec3( 0, 0, 1 ) );
		break;

	case InputAction::CAMERA_POS_3:
		s_player->SetPosition( cube_pos - glm::vec3( 0, 0, 2 ) );
		break;

	case InputAction::CAMERA_POS_4:
		s_player->SetPosition( cube_pos - glm::vec3( 0, 0, 3 ) );
		break;

	case InputAction::DECREASE_DEPTH:
		s_player->SetPosition( s_player->GetPosition() + glm::vec3( 0, 0, 1 ) );
		break;

	case InputAction::INCREASE_DEPTH:
		s_player->SetPosition( s_player->GetPosition() - glm::vec3( 0, 0, 1 ) );
		break;
	}
}

void SetTimeScale( InputAction action, InputType type )
{
	if( action == InputAction::TIME_SCALE_DOWN && type == InputType::RELEASED )
	{
		float time_scale = s_frameTimer->GetTimeScale();
		s_frameTimer->SetTimeScale( time_scale * 0.9f );
	}

	if( action == InputAction::TIME_SCALE_UP && type == InputType::RELEASED )
	{
		float time_scale = s_frameTimer->GetTimeScale();
		s_frameTimer->SetTimeScale( time_scale * 1.1f );
	}
}

void UpdateFreeCam( FreeCameraController& free_cam, ShakyCamera& shaky_cam, InputSystem& input, float delta_t )
{
	bool captureMouse = !s_debugUI->Draw();
	if( captureMouse != input.Source().IsMouseCaptured() )
	{
		input.Source().CaptureMouse( captureMouse );
	}

	if( captureMouse )
	{
		free_cam.UpdateMouse( input.Source().GetMousePosition() );
		free_cam.UpdateScroll( input.Source().GetScrollPosition() );
	}

	shaky_cam.Update( delta_t );
}

void CreateSingletons()
{
	ddr::TextureManager::RegisterSingleton( new ddr::TextureManager() );
	ddr::ShaderManager::RegisterSingleton( new ddr::ShaderManager() );
	ddr::MaterialManager::RegisterSingleton( new ddr::MaterialManager() );
	ddr::MeshManager::RegisterSingleton( new ddr::MeshManager() );
	ddc::EntityPrototypeManager::RegisterSingleton( new ddc::EntityPrototypeManager() );
}

void UpdateSingletons()
{
	ddr::TextureManager::Instance()->Update();
	ddr::ShaderManager::Instance()->Update();
	ddr::MaterialManager::Instance()->Update();
	ddr::MeshManager::Instance()->Update();
	ddc::EntityPrototypeManager::Instance()->Update();
}

ddc::Entity CreateMeshEntity( ddc::World& world, ddr::MeshHandle mesh_h, glm::vec4 colour, glm::vec3 pos, glm::quat rot, glm::vec3 scale )
{
	ddc::Entity entity = world.CreateEntity<dd::TransformComponent, dd::MeshComponent, dd::BoundBoxComponent, dd::ColourComponent>();

	dd::TransformComponent* transform_cmp = world.Access<dd::TransformComponent>( entity );
	transform_cmp->Position = pos;
	transform_cmp->Scale = scale;
	transform_cmp->Rotation = rot;
	transform_cmp->Update();

	dd::MeshComponent* mesh_cmp = world.Access<dd::MeshComponent>( entity );
	mesh_cmp->Mesh = mesh_h;

	dd::ColourComponent* colour_cmp = world.Access<dd::ColourComponent>( entity );
	colour_cmp->Colour = colour;

	dd::BoundBoxComponent* bounds_cmp = world.Access<dd::BoundBoxComponent>( entity );
	bounds_cmp->BoundBox = ddr::MeshManager::Instance()->Get( mesh_h )->GetBoundBox();

	world.AddTag( entity, ddc::Tag::Visible );

	return entity;
}

ddc::Entity CreateBall( glm::vec3 translation, glm::vec4 colour, float size )
{
	ddr::MeshHandle mesh_h = ddr::MeshManager::Instance()->Find( "sphere" );

	ddc::Entity entity = CreateMeshEntity( *s_world, mesh_h, colour,
		translation,
		glm::angleAxis( glm::radians( 45.0f ), glm::vec3( 0, 1, 0 ) ),
		glm::vec3( size ) );

	s_world->AddTag( entity, ddc::Tag::Dynamic );

	dd::BoundSphereComponent& bound_sphere = s_world->Add<dd::BoundSphereComponent>( entity );
	bound_sphere.Sphere.Radius = 1.0f;

	dd::PhysicsSphereComponent& physics_sphere = s_world->Add<dd::PhysicsSphereComponent>( entity );
	physics_sphere.Sphere.Radius = 1.0f;
	physics_sphere.Elasticity = 0.95f;
	physics_sphere.Mass = 1.0f;

	return entity;
}

void CreatePhysicsPlaneTestScene()
{
	float sphere_size = 3;

	dd::Array<glm::vec3, 4> ball_positions;
	ball_positions.Add( glm::vec3( 0, 60, -30 ) );
	ball_positions.Add( glm::vec3( 0, 60, 30 ) );
	ball_positions.Add( glm::vec3( 30, 60, 0 ) );
	ball_positions.Add( glm::vec3( -30, 60, 0 ) );

	dd::Array<ddc::Entity, 4> balls;
	balls.Add( CreateBall( ball_positions[0], glm::vec4( 0.2, 0.2, 0.8, 1 ), sphere_size ) );
	balls.Add( CreateBall( ball_positions[1], glm::vec4( 0.8, 0.2, 0.8, 1 ), sphere_size ) );
	balls.Add( CreateBall( ball_positions[2], glm::vec4( 0.2, 0.8, 0.2, 1 ), sphere_size ) );
	balls.Add( CreateBall( ball_positions[3], glm::vec4( 0.2, 0.8, 0.8, 1 ), sphere_size ) );

	float static_sphere_size = 100;

	/* STATIC SPHERES
	{
		ddc::Entity sphere = CreateMeshEntity( *s_world, ddr::MeshManager::Instance()->Find( "sphere" ), glm::vec4( 0.9, 0.9, 0.9, 1 ), glm::vec3( 0, static_sphere_size * -0.7f, 0 ), glm::quat(), glm::vec3( static_sphere_size ) );
		s_world->AddTag( sphere, ddc::Tag::Static );

		dd::PhysicsSphereComponent& physics_sphere = s_world->Add<dd::PhysicsSphereComponent>( sphere );
		physics_sphere.Sphere = ddm::Sphere( glm::vec3( 0, 0, 0 ), 1 );
		physics_sphere.Elasticity = 0.95f;
	}

	{
		ddc::Entity sphere = CreateMeshEntity( *s_world, ddr::MeshManager::Instance()->Find( "sphere" ), glm::vec4( 0.9, 0.9, 0.9, 1 ), glm::vec3( 0, static_sphere_size * -0.7f, -static_sphere_size * 1.3f ), glm::quat(), glm::vec3( static_sphere_size ) );
		s_world->AddTag( sphere, ddc::Tag::Static );

		dd::PhysicsSphereComponent& physics_sphere = s_world->Add<dd::PhysicsSphereComponent>( sphere );
		physics_sphere.Sphere = ddm::Sphere( glm::vec3( 0, 0, 0 ), 1 );
		physics_sphere.Elasticity = 0.95f;
	}

	{
		ddc::Entity sphere = CreateMeshEntity( *s_world, ddr::MeshManager::Instance()->Find( "sphere" ), glm::vec4( 0.9, 0.9, 0.9, 1 ), glm::vec3( 0, static_sphere_size * -0.7f, static_sphere_size * 1.3f ), glm::quat(), glm::vec3( static_sphere_size ) );
		s_world->AddTag( sphere, ddc::Tag::Static );

		dd::PhysicsSphereComponent& physics_sphere = s_world->Add<dd::PhysicsSphereComponent>( sphere );
		physics_sphere.Sphere = ddm::Sphere( glm::vec3( 0, 0, 0 ), 1 );
		physics_sphere.Elasticity = 0.95f;
	}*/

	float plane_size = 100;
	auto proto_h = ddc::EntityPrototypeManager::Instance()->Create( "physics_plane" );
	ddc::EntityPrototype* phys_plane_proto = proto_h.Access();

	{
		ddc::Entity plane = CreateMeshEntity( *s_world, ddr::MeshManager::Instance()->Find( "quad" ), glm::vec4( 0.2, 0.8, 0.2, 1 ), glm::vec3( 0 ), glm::angleAxis( glm::radians( 45.0f ), glm::vec3( 1, 0, 0 ) ), glm::vec3( plane_size ) );
		s_world->AddTag( plane, ddc::Tag::Static );

		dd::PhysicsPlaneComponent& physics_plane = s_world->Add<dd::PhysicsPlaneComponent>( plane );
		physics_plane.Plane = ddm::Plane( glm::vec3( 0, 0, 0 ), glm::vec3( 0, 1, 0 ) );
		physics_plane.Elasticity = 0.95f;

		phys_plane_proto->PopulateFromEntity( plane, *s_world );
	}

	{
		ddc::Entity plane = phys_plane_proto->Instantiate( *s_world );
		dd::ColourComponent* clr = s_world->Access<dd::ColourComponent>( plane );
		clr->Colour = glm::vec4( 0.8, 0.2, 0.2, 1 );

		dd::TransformComponent* transform_cmp = s_world->Access<dd::TransformComponent>( plane );
		transform_cmp->Rotation = glm::angleAxis( glm::radians( -45.0f ), glm::vec3( 1, 0, 0 ) );
		transform_cmp->Update();
	}

	{
		ddc::Entity plane = phys_plane_proto->Instantiate( *s_world );
		dd::ColourComponent* clr = s_world->Access<dd::ColourComponent>( plane );
		clr->Colour = glm::vec4( 0.8, 0.8, 0.2, 1 );

		dd::TransformComponent* transform_cmp = s_world->Access<dd::TransformComponent>( plane );
		transform_cmp->Rotation = glm::angleAxis( glm::radians( 90.0f ), glm::vec3( 0, 1, 0 ) ) *
			glm::angleAxis( glm::radians( 45.0f ), glm::vec3( 1, 0, 0 ) );

		transform_cmp->Update();
	}

	{
		ddc::Entity plane = phys_plane_proto->Instantiate( *s_world );
		dd::ColourComponent* clr = s_world->Access<dd::ColourComponent>( plane );
		clr->Colour = glm::vec4( 0.2, 0.2, 0.8, 1 );

		dd::TransformComponent* transform_cmp = s_world->Access<dd::TransformComponent>( plane );
		transform_cmp->Rotation = glm::angleAxis( glm::radians( -90.0f ), glm::vec3( 0, 1, 0 ) ) *
			glm::angleAxis( glm::radians( 45.0f ), glm::vec3( 1, 0, 0 ) );

		transform_cmp->Update();
	}

	ddc::World* world = s_world;
	Service<InputBindings>()->RegisterHandler( InputAction::RESET_PHYSICS, [balls, ball_positions, world]( InputAction action, InputType type )
	{
		if( type == InputType::RELEASED )
		{
			for( size_t i = 0; i < balls.Size(); ++i )
			{
				dd::TransformComponent* transform = world->Access<dd::TransformComponent>( balls[i] );
				transform->Position = ball_positions[i];
				transform->Update();

				dd::PhysicsSphereComponent* sphere = world->Access<dd::PhysicsSphereComponent>( balls[i] );
				sphere->Momentum = glm::vec3( 0, 0, 0 );
				sphere->Resting = false;
			}
		}
	} );
}

void CreateAxes()
{
	{
		ddc::Entity x_entity = s_world->CreateEntity<dd::RayComponent, dd::ColourComponent>();
		s_world->AddTag( x_entity, ddc::Tag::Visible );

		dd::RayComponent* x_ray = s_world->Access<dd::RayComponent>( x_entity );
		x_ray->Ray = ddm::Ray( glm::vec3( -50, 0, 0 ), glm::vec3( 1, 0, 0 ), 100 );

		dd::ColourComponent* x_colour = s_world->Access<dd::ColourComponent>( x_entity );
		x_colour->Colour = glm::vec4( 1, 0, 0, 1 );
	}

	{
		ddc::Entity y_entity = s_world->CreateEntity<dd::RayComponent, dd::ColourComponent>();
		s_world->AddTag( y_entity, ddc::Tag::Visible );

		dd::RayComponent* y_ray = s_world->Access<dd::RayComponent>( y_entity );
		y_ray->Ray = ddm::Ray( glm::vec3( 0, -50, 0 ), glm::vec3( 0, 1, 0 ), 100 );

		dd::ColourComponent* y_colour = s_world->Access<dd::ColourComponent>( y_entity );
		y_colour->Colour = glm::vec4( 0, 1, 0, 1 );
	}

	{
		ddc::Entity z_entity = s_world->CreateEntity<dd::RayComponent, dd::ColourComponent>();
		s_world->AddTag( z_entity, ddc::Tag::Visible );

		dd::RayComponent* z_ray = s_world->Access<dd::RayComponent>( z_entity );
		z_ray->Ray = ddm::Ray( glm::vec3( 0, 0, -50 ), glm::vec3( 0, 0, 1 ), 100 );

		dd::ColourComponent* z_colour = s_world->Access<dd::ColourComponent>( z_entity );
		z_colour->Colour = glm::vec4( 0, 0, 1, 1 );
	}
}

int GameMain()
{
	DD_PROFILE_INIT();
	DD_PROFILE_THREAD_NAME( "Main" );

	dd::TypeInfo::SetScriptEngine( new dd::AngelScriptEngine() );

	dd::TypeInfo::RegisterQueuedTypes();

	unsigned int threads = std::thread::hardware_concurrency();
	dd::JobSystem* jobsystem = new JobSystem( threads - 1 );

	s_mainThread = std::this_thread::get_id();

	{
		Services::Register( new Window( glm::ivec2( 1920, 1080 ), "DD" ) );

		GLFWInputSource* input_source = new GLFWInputSource( *s_window );
		input_source->SetMode( InputMode::GAME );

		Services::Register( new InputBindings() );
		s_inputBindings->RegisterHandler( InputAction::TOGGLE_FREECAM, &ToggleFreeCam );
		s_inputBindings->RegisterHandler( InputAction::TOGGLE_DEBUG_UI, &ToggleDebugUI );
		s_inputBindings->RegisterHandler( InputAction::EXIT, &Exit );
		s_inputBindings->RegisterHandler( InputAction::PAUSE, &PauseGame );
		s_inputBindings->RegisterHandler( InputAction::CAMERA_POS_1, &SetCameraPos );
		s_inputBindings->RegisterHandler( InputAction::CAMERA_POS_2, &SetCameraPos );
		s_inputBindings->RegisterHandler( InputAction::CAMERA_POS_3, &SetCameraPos );
		s_inputBindings->RegisterHandler( InputAction::CAMERA_POS_4, &SetCameraPos );
		s_inputBindings->RegisterHandler( InputAction::INCREASE_DEPTH, &SetCameraPos );
		s_inputBindings->RegisterHandler( InputAction::DECREASE_DEPTH, &SetCameraPos );
		s_inputBindings->RegisterHandler( InputAction::TIME_SCALE_DOWN, &SetTimeScale );
		s_inputBindings->RegisterHandler( InputAction::TIME_SCALE_UP, &SetTimeScale );

		Services::Register( new InputSystem( *input_source, *s_inputBindings ) );
		s_input->BindKeys();

		Services::Register( new DebugUI( *s_window, *input_source ) );

		SwarmSystem* swarm_system = new SwarmSystem();

		//TrenchSystem trench_system( *s_shakyCam  );
		//trench_system.CreateRenderResources();

		Services::Register( new FreeCameraController() );
		s_freeCamera->BindActions( *s_inputBindings );

		HitTestSystem* hit_testing = new HitTestSystem();
		hit_testing->DependsOn( *s_freeCamera );

		PhysicsSystem* physics_system = new PhysicsSystem();

		//ShipSystem ship_system( *s_shakyCam  );
		//s_shipSystem = &ship_system;
		//s_shipSystem->BindActions( bindings );
		//s_shipSystem->CreateShip( *s_world );

		TerrainSystem* terrain_system = new TerrainSystem( *jobsystem );

		BulletSystem* bullet_system = new BulletSystem( *hit_testing );
		bullet_system->DependsOn( *s_freeCamera );
		bullet_system->DependsOn( *hit_testing );
		bullet_system->BindActions( *s_inputBindings );

		dd::ParticleSystem* particle_system = new dd::ParticleSystem();
		particle_system->BindActions( *s_inputBindings );

		dd::TreeSystem* tree_system = new dd::TreeSystem();

		dd::WaterSystem* water_system = new dd::WaterSystem( terrain_system->GetTerrainParameters(), *jobsystem );
		water_system->DependsOn( *terrain_system );

		Services::Register( new ddc::World( *jobsystem ) );
		
		s_world->RegisterSystem( *s_freeCamera );
		s_world->RegisterSystem( *terrain_system );
		s_world->RegisterSystem( *particle_system );
		s_world->RegisterSystem( *hit_testing );
		s_world->RegisterSystem( *bullet_system );
		s_world->RegisterSystem( *physics_system );
		s_world->RegisterSystem( *swarm_system );
		s_world->RegisterSystem( *tree_system );
		s_world->RegisterSystem( *water_system );

		Services::Register( new ddr::WorldRenderer( *s_window ) );

		ddr::ParticleSystemRenderer* particle_renderer = new ddr::ParticleSystemRenderer();

		MousePicking* mouse_picking = new MousePicking( *s_window, *input_source, *hit_testing );
		mouse_picking->BindActions( *s_inputBindings );

		ddr::MeshRenderer* mesh_renderer = new ddr::MeshRenderer( *jobsystem );
		ddr::LightRenderer* light_renderer = new ddr::LightRenderer();

		ddr::BoundsRenderer* bounds_renderer = new ddr::BoundsRenderer();
		bounds_renderer->BindKeys( *s_inputBindings );

		ddr::RayRenderer* ray_renderer = new ddr::RayRenderer();

		ddr::LinesRenderer* lines_renderer = new ddr::LinesRenderer();

		ddr::TerrainRenderer* terrain_renderer = new ddr::TerrainRenderer( terrain_system->GetTerrainParameters() );

		ddr::WaterRenderer* water_renderer = new ddr::WaterRenderer( *jobsystem );

		s_renderer->Register( *mouse_picking );
		s_renderer->Register( *light_renderer );
		s_renderer->Register( *terrain_renderer );
		s_renderer->Register( *particle_renderer );
		s_renderer->Register( *mesh_renderer );
		s_renderer->Register( *bounds_renderer );
		s_renderer->Register( *ray_renderer );
		s_renderer->Register( *lines_renderer );
		s_renderer->Register( *water_renderer );
		
		Services::Register( new FrameTimer() );
		s_frameTimer->SetMaxFPS( s_maxFPS );

		dd::EntityVisualizer* entity_visualizer = new dd::EntityVisualizer();
		entity_visualizer->BindActions( *s_inputBindings );

		s_debugUI->RegisterDebugPanel( *s_frameTimer );
		s_debugUI->RegisterDebugPanel( *s_renderer );
		s_debugUI->RegisterDebugPanel( *s_freeCamera );
		s_debugUI->RegisterDebugPanel( *mouse_picking );
		s_debugUI->RegisterDebugPanel( *particle_system );
		s_debugUI->RegisterDebugPanel( *terrain_system );
		s_debugUI->RegisterDebugPanel( *mesh_renderer );
		s_debugUI->RegisterDebugPanel( *bounds_renderer );
		s_debugUI->RegisterDebugPanel( *light_renderer );
		s_debugUI->RegisterDebugPanel( *hit_testing );
		s_debugUI->RegisterDebugPanel( *bullet_system );
		s_debugUI->RegisterDebugPanel( *physics_system );
		s_debugUI->RegisterDebugPanel( *s_world );
		s_debugUI->RegisterDebugPanel( *entity_visualizer );
		s_debugUI->RegisterDebugPanel( *terrain_renderer );
		s_debugUI->RegisterDebugPanel( *swarm_system );
		s_debugUI->RegisterDebugPanel( *tree_system );
		s_debugUI->RegisterDebugPanel( *water_system );

		CreateSingletons();

		dd::CreateDefaultMaterial();
		dd::CreateUnitCube();
		dd::CreateUnitSphere();
		dd::CreateQuad();

		s_world->Initialize();

		s_renderer->InitializeRenderers( *s_world );

		// player
		{
			ddc::Entity entity = s_world->CreateEntity<dd::TransformComponent, dd::PlayerComponent, dd::FPSCameraComponent>();

			Services::Register( s_world->Access<dd::FPSCameraComponent>( entity ) );
			s_player->SetAspectRatio( s_window->GetWidth(), s_window->GetHeight() );
			s_player->SetVerticalFOV( glm::radians( 45.0f ) );

			DD_TODO( "Shaky camera should be a component/system pair." );
			Services::Register( new ShakyCamera( s_player, s_inputBindings ) );

			s_debugUI->RegisterDebugPanel( Service<ShakyCamera>::Get() );

			dd::TransformComponent* transform = s_world->Access<dd::TransformComponent>( entity );

		}

		// dir light
		{
			ddc::Entity entity = s_world->CreateEntity<dd::LightComponent, dd::TransformComponent>();
			s_world->AddTag( entity, ddc::Tag::Visible );

			dd::LightComponent* light = s_world->Access<dd::LightComponent>( entity );
			light->LightType = dd::LightType::Directional;
			light->Colour = glm::vec3( 1, 1, 1 );
			light->Intensity = 0.7;

			dd::TransformComponent* transform = s_world->Access<dd::TransformComponent>( entity );
			transform->Rotation = glm::angleAxis( glm::radians( 45.0f ), glm::vec3( 1, 1, 0 ) );
			transform->Update();
		}

		// point light
		{
			ddc::Entity entity = s_world->CreateEntity<dd::LightComponent, dd::TransformComponent>();
			s_world->AddTag( entity, ddc::Tag::Visible );

			dd::LightComponent* light = s_world->Access<dd::LightComponent>( entity );
			light->LightType = dd::LightType::Point;
			light->Colour = glm::vec3( 1, 1, 1 );
			light->Intensity = 3;
			light->Ambient = 0.01;
			light->Attenuation = 0.03;
			light->InnerAngle = glm::radians( 30.f );
			light->OuterAngle = glm::radians( 45.f );

			dd::TransformComponent* transform = s_world->Access<dd::TransformComponent>( entity );
			transform->Position = glm::vec3( 0, 30, 0 );
			transform->Rotation = glm::angleAxis( glm::radians( 45.0f ), glm::vec3( 1, 0, 0 ) );
			transform->Update();
		}

		// particle system
		/*{
			ddc::Entity entity = s_world->CreateEntity<dd::ParticleSystemComponent, dd::TransformComponent, dd::BoundBoxComponent>();
			s_world->AddTag( entity, ddc::Tag::Visible );
			s_world->AddTag( entity, ddc::Tag::Dynamic );

			dd::TransformComponent* transform = s_world->Access<dd::TransformComponent>( entity );
			transform->Position = glm::vec3( 10, 60, 10 );
			transform->Update();

			dd::BoundBoxComponent* bounds = s_world->Access<dd::BoundBoxComponent>( entity );
			bounds->BoundBox = ddm::AABB( glm::vec3( -0.5 ), glm::vec3( 0.5 ) );

			dd::ParticleSystemComponent* particle = s_world->Access<dd::ParticleSystemComponent>( entity );
			particle->Age = 0;
			particle->Lifetime = 1000;
		}*/

		// axes
		CreateAxes();

		// physics
		//CreatePhysicsPlaneTestScene();

		// everything's set up, so we can start using ImGui - asserts before this will be handled by the default console
		dd::InitializeAssert( s_mainThread );
		::ShowWindow( GetConsoleWindow(), SW_HIDE );

		while( !s_window->ShouldClose() )
		{
			DD_PROFILE_SCOPED( Frame );

			s_frameTimer->SetMaxFPS( s_maxFPS );
			s_frameTimer->Update();

			float game_delta_t = s_frameTimer->GameDelta();
			float app_delta_t = s_frameTimer->AppDelta();

			s_input->Update( app_delta_t );
			s_debugUI->StartFrame( s_frameTimer->AppDelta() );

			s_world->Update( game_delta_t );

			UpdateSingletons();

			s_player->SetAspectRatio( s_window->GetWidth(), s_window->GetHeight() );

			UpdateFreeCam( s_freeCamera, s_shakyCamera, s_input, app_delta_t );

			s_debugUI->RenderDebugPanels( s_world );
			s_frameTimer->DrawFPSCounter();

			s_renderer->Render( s_world, s_shakyCamera, game_delta_t );

			s_debugUI->EndFrame();
			s_window->Swap();

			s_frameTimer->DelayFrame();
		}
	}

	DD_PROFILE_DEINIT();

	return 0;
}

#ifdef _TEST

int TestMain( int argc, char* argv[] )
{
	s_scriptEngine = new AngelScriptEngine();
	dd::TypeInfo::SetScriptEngine( s_scriptEngine );

	dd::TypeInfo::RegisterQueuedTypes();

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

#ifdef _TEST
	return TestMain( argc, argv );
#else
	return GameMain();
#endif
}
