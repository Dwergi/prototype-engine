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
#include "DebugConsole.h"
#include "DebugUI.h"
#include "DDAssertHelpers.h"
#include "DoubleBuffer.h"
#include "File.h"
#include "FrameTimer.h"
#include "FreeCameraController.h"
#include "FSM.h"
#include "FPSCamera.h"
#include "GLFWInputSource.h"
#include "HitTestSystem.h"
#include "IDebugPanel.h"
#include "IInputSource.h"
#include "InputBindings.h"
#include "InputSystem.h"
#include "JobSystem.h"
#include "LightComponent.h"
#include "LightRenderer.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MeshRenderer.h"
#include "MeshUtils.h"
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
#include "PhysicsPlaneComponent.h"
#include "PhysicsSphereComponent.h"
#include "PhysicsSystem.h"
#include "RayComponent.h"
#include "ScopedTimer.h"
#include "ScriptComponent.h"
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
#include "TrenchSystem.h"
#include "Uniforms.h"
#include "Window.h"
#include "World.h"

#include "SFML/Network/UdpSocket.hpp"

#include <chrono>
#include <thread>
//---------------------------------------------------------------------------

using namespace dd;

extern uint s_maxFPS;
uint s_maxFPS = 60;

DebugUI* s_debugUI = nullptr;
Window* s_window = nullptr;
InputSystem* s_input = nullptr;
ShakyCamera* s_shakyCamera = nullptr;
FPSCamera* s_fpsCamera = nullptr;
FreeCameraController* s_freeCamera = nullptr;
ddc::World* s_world = nullptr;
AngelScriptEngine* s_scriptEngine = nullptr;
FrameTimer* s_frameTimer = nullptr;
ShipSystem* s_shipSystem = nullptr;
FSM* s_fsm = nullptr;
InputBindings* input_bindings = nullptr;
DebugConsole* s_debugConsole = nullptr;

ddr::WorldRenderer* s_renderer = nullptr;

Assert s_assert;

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
		s_debugUI->EnableDraw( !s_debugUI->Draw() );
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

void SetCameraPos( InputAction action, InputType type )
{
	if( type != InputType::RELEASED )
		return;

	s_fpsCamera->SetRotation( 0, 0 );

	const glm::vec3 cube_pos( 10.5, 60.5, 10 );

	switch( action )
	{
	case InputAction::CAMERA_POS_1:
		s_fpsCamera->SetPosition( cube_pos - glm::vec3( 0, 0, 0.5 ) );
		break;

	case InputAction::CAMERA_POS_2:
		s_fpsCamera->SetPosition( cube_pos - glm::vec3( 0, 0, 1 ) );
		break;

	case InputAction::CAMERA_POS_3:
		s_fpsCamera->SetPosition( cube_pos - glm::vec3( 0, 0, 2 ) );
		break;

	case InputAction::CAMERA_POS_4:
		s_fpsCamera->SetPosition( cube_pos - glm::vec3( 0, 0, 3 ) );
		break;

	case InputAction::DECREASE_DEPTH:
		s_fpsCamera->SetPosition( s_fpsCamera->GetPosition() + glm::vec3( 0, 0, 1 ) );
		break;

	case InputAction::INCREASE_DEPTH:
		s_fpsCamera->SetPosition( s_fpsCamera->GetPosition() - glm::vec3( 0, 0, 1 ) );
		break;
	}
}

void CheckAssert()
{
	if( s_assert.Open )
	{
		static dd::String256 s_message;
		s_message = s_assert.Info;
		s_message += s_assert.Message;

		printf( s_message.c_str() );
		OutputDebugStringA( s_message.c_str() );

		s_input->Source().CaptureMouse( false );

		if( s_debugUI->IsMidWindow() )
		{
			ImGui::End();
		}

		if( s_debugUI->IsMidFrame() )
		{
			s_debugUI->EndFrame();
			s_window->Swap();
		}

		do
		{
			s_frameTimer->Update();

			float delta_t = s_frameTimer->AppDelta();

			s_input->Update( delta_t );

			s_debugUI->StartFrame( delta_t );

			DrawAssertDialog( s_window->GetSize(), s_assert );

			s_debugUI->EndFrame();
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
		free_cam.Update( delta_t );
	}

	shaky_cam.Update( delta_t );
}

ddc::Entity CreateMeshEntity( ddc::World& world, ddr::MeshHandle mesh_h, glm::vec4 colour, const glm::mat4& transform )
{
	ddc::Entity entity = world.CreateEntity<dd::TransformComponent, dd::MeshComponent, dd::BoundBoxComponent, dd::ColourComponent>();

	dd::TransformComponent* transform_cmp = world.Access<dd::TransformComponent>( entity );
	transform_cmp->Transform = transform;

	dd::MeshComponent* mesh_cmp = world.Access<dd::MeshComponent>( entity );
	mesh_cmp->Mesh = mesh_h;

	dd::ColourComponent* colour_cmp = world.Access<dd::ColourComponent>( entity );
	colour_cmp->Colour = colour;

	dd::BoundBoxComponent* bounds_cmp = world.Access<dd::BoundBoxComponent>( entity );
	bounds_cmp->BoundBox = ddr::Mesh::Get( mesh_h )->GetBoundBox();

	world.AddTag( entity, ddc::Tag::Visible );

	return entity;
}

void CreateUnitCube()
{
	ddr::MeshHandle unitCube = ddr::Mesh::Find( "cube" );
	if( !unitCube.IsValid() )
	{
		unitCube = ddr::Mesh::Create( "cube" );

		ddr::Mesh* mesh = ddr::Mesh::Get( unitCube );
		DD_ASSERT( mesh != nullptr );

		ddr::ShaderHandle shader_h = ddr::ShaderProgram::Load( "mesh" );
		ddr::ShaderProgram* shader = ddr::ShaderProgram::Get( shader_h );
		DD_ASSERT( shader != nullptr );

		ddr::MaterialHandle material_h = ddr::Material::Create( "mesh" );
		ddr::Material* material = ddr::Material::Get( material_h );
		DD_ASSERT( material != nullptr );

		material->SetShader( shader_h );
		mesh->SetMaterial( material_h );

		shader->Use( true );

		dd::MakeUnitCube( *mesh );

		shader->Use( false );
	}
}

void CreateUnitSphere()
{
	ddr::MeshHandle unitSphere = ddr::Mesh::Find( "sphere" );
	if( !unitSphere.IsValid() )
	{
		unitSphere = ddr::Mesh::Create( "sphere" );

		ddr::Mesh* mesh = ddr::Mesh::Get( unitSphere );
		DD_ASSERT( mesh != nullptr );

		ddr::ShaderHandle shader_h = ddr::ShaderProgram::Load( "mesh" );
		ddr::ShaderProgram* shader = ddr::ShaderProgram::Get( shader_h );
		DD_ASSERT( shader != nullptr );

		ddr::MaterialHandle material_h = ddr::Material::Create( "mesh" );
		ddr::Material* material = ddr::Material::Get( material_h );
		DD_ASSERT( material != nullptr );

		material->SetShader( shader_h );
		mesh->SetMaterial( material_h );

		shader->Use( true );

		dd::MakeIcosphere( *mesh, 2 );

		shader->Use( false );
	}
}

void CreateQuad()
{
	ddr::MeshHandle quad = ddr::Mesh::Find( "quad" );
	if( !quad.IsValid() )
	{
		quad = ddr::Mesh::Create( "quad" );

		ddr::Mesh* mesh = ddr::Mesh::Get( quad );
		DD_ASSERT( mesh != nullptr );

		ddr::ShaderHandle shader_h = ddr::ShaderProgram::Load( "mesh" );
		ddr::ShaderProgram* shader = ddr::ShaderProgram::Get( shader_h );
		DD_ASSERT( shader != nullptr );

		ddr::MaterialHandle material_h = ddr::Material::Create( "mesh" );
		ddr::Material* material = ddr::Material::Get( material_h );
		DD_ASSERT( material != nullptr );

		material->SetShader( shader_h );
		mesh->SetMaterial( material_h );

		shader->Use( true );

		dd::MakeQuad( *mesh );

		shader->Use( false );
	}
}

ddc::Entity CreateBall( glm::vec3 translation, glm::vec4 colour, float size )
{
	ddr::MeshHandle mesh_h = ddr::Mesh::Find( "sphere" );

	ddc::Entity entity = CreateMeshEntity( *s_world, mesh_h, colour,
		glm::translate( translation ) * 
		glm::rotate( glm::radians( 45.0f ), glm::vec3( 0, 1, 0 ) ) * 
		glm::scale( glm::vec3( size ) ) );

	s_world->AddTag( entity, ddc::Tag::Dynamic );

	dd::PhysicsSphereComponent& physics_sphere = s_world->Add<dd::PhysicsSphereComponent>( entity );
	physics_sphere.Sphere.Radius = 1.0f;
	physics_sphere.Elasticity = 1.0f;
	physics_sphere.Mass = 1.0f;

	return entity;
}

int GameMain()
{
	DD_PROFILE_INIT();
	DD_PROFILE_THREAD_NAME( "Main" );

	dd::TypeInfo::SetScriptEngine( new dd::AngelScriptEngine() );

	dd::TypeInfo::RegisterDefaultTypes();
	dd::TypeInfo::RegisterQueuedTypes();

	unsigned int threads = std::thread::hardware_concurrency();
	JobSystem jobsystem( threads - 1 );

	s_mainThread = std::this_thread::get_id();

	{
		s_window = new Window( glm::ivec2( 1920, 1080 ), "DD" );

		GLFWInputSource* input_source = new GLFWInputSource( *s_window );

		InputBindings* input_bindings = new InputBindings();
		input_bindings->RegisterHandler( InputAction::TOGGLE_FREECAM, &ToggleFreeCam );
		input_bindings->RegisterHandler( InputAction::TOGGLE_DEBUG_UI, &ToggleDebugUI );
		input_bindings->RegisterHandler( InputAction::EXIT, &Exit );
		input_bindings->RegisterHandler( InputAction::BREAK, &TriggerAssert );
		input_bindings->RegisterHandler( InputAction::CAMERA_POS_1, &SetCameraPos );
		input_bindings->RegisterHandler( InputAction::CAMERA_POS_2, &SetCameraPos );
		input_bindings->RegisterHandler( InputAction::CAMERA_POS_3, &SetCameraPos );
		input_bindings->RegisterHandler( InputAction::CAMERA_POS_4, &SetCameraPos );
		input_bindings->RegisterHandler( InputAction::INCREASE_DEPTH, &SetCameraPos );
		input_bindings->RegisterHandler( InputAction::DECREASE_DEPTH, &SetCameraPos );

		s_input = new InputSystem( *input_source, *input_bindings );
		s_input->BindKeys();

		s_debugUI = new DebugUI( *s_window, *input_source );

		//SwarmSystem swarm_system;

		//TrenchSystem trench_system( *s_shakyCam  );
		//trench_system.CreateRenderResources();

		s_fpsCamera = new FPSCamera( *s_window );
		s_fpsCamera->SetPosition( glm::vec3( 0, 20, 0 ) );
		s_fpsCamera->SetRotation( 0, 0 );

		s_shakyCamera = new ShakyCamera( *s_fpsCamera, *input_bindings );
		
		s_freeCamera = new FreeCameraController( *s_fpsCamera );
		s_freeCamera->BindActions( *input_bindings );

		HitTestSystem* hit_testing = new HitTestSystem();

		MousePicking* mouse_picking = new MousePicking( *s_window, *input_source, *hit_testing );
		mouse_picking->BindActions( *input_bindings );

		PhysicsSystem* physics_system = new PhysicsSystem();
		
		//ShipSystem ship_system( *s_shakyCam  );
		//s_shipSystem = &ship_system;
		//s_shipSystem->BindActions( bindings );
		//s_shipSystem->CreateShip( *s_world );

		TerrainSystem* terrain_system = new TerrainSystem( jobsystem );

		BulletSystem* bullet_system = new BulletSystem( *s_fpsCamera, *hit_testing );
		bullet_system->DependsOn( *hit_testing );
		bullet_system->BindActions( *input_bindings );

		ddr::ParticleSystem* particle_system = new ddr::ParticleSystem();
		particle_system->BindActions( *input_bindings );

		s_world = new ddc::World( jobsystem );

		//s_world->RegisterSystem( *terrain_system );
		s_world->RegisterSystem( *particle_system );
		s_world->RegisterSystem( *hit_testing );
		s_world->RegisterSystem( *bullet_system );
		s_world->RegisterSystem( *physics_system );

		s_renderer = new ddr::WorldRenderer( *s_window );

		ddr::ParticleSystemRenderer* particle_renderer = new ddr::ParticleSystemRenderer();

		ddr::MeshRenderer* mesh_renderer = new ddr::MeshRenderer( *mouse_picking );

		ddr::LightRenderer* light_renderer = new ddr::LightRenderer();

		ddr::BoundsRenderer* bounds_renderer = new ddr::BoundsRenderer();

		ddr::RayRenderer* ray_renderer = new ddr::RayRenderer();

		ddr::TerrainRenderer* terrain_renderer = new ddr::TerrainRenderer( terrain_system->GetTerrainParameters() );

		s_renderer->Register( *mouse_picking );
		s_renderer->Register( *light_renderer );
		s_renderer->Register( *terrain_renderer );
		s_renderer->Register( *particle_renderer );
		s_renderer->Register( *mesh_renderer );
		s_renderer->Register( *bounds_renderer );
		s_renderer->Register( *ray_renderer );

		s_frameTimer = new FrameTimer();
		s_frameTimer->SetMaxFPS( s_maxFPS );

		s_debugUI->RegisterDebugPanel( *s_frameTimer );
		s_debugUI->RegisterDebugPanel( *s_renderer );
		s_debugUI->RegisterDebugPanel( *s_freeCamera );
		s_debugUI->RegisterDebugPanel( *mouse_picking );
		s_debugUI->RegisterDebugPanel( *s_shakyCamera );
		s_debugUI->RegisterDebugPanel( *particle_system );
		s_debugUI->RegisterDebugPanel( *terrain_system );
		s_debugUI->RegisterDebugPanel( *mesh_renderer );
		s_debugUI->RegisterDebugPanel( *bounds_renderer );
		s_debugUI->RegisterDebugPanel( *light_renderer );
		s_debugUI->RegisterDebugPanel( *hit_testing );
		s_debugUI->RegisterDebugPanel( *bullet_system );
		s_debugUI->RegisterDebugPanel( *physics_system );
		s_debugUI->RegisterDebugPanel( *s_world );

		s_world->Initialize();

		CreateUnitCube();
		CreateUnitSphere();
		CreateQuad();

		s_renderer->InitializeRenderers( *s_world );

		// dir light
		{
			ddc::Entity entity = s_world->CreateEntity<dd::LightComponent, dd::TransformComponent>();
			s_world->AddTag( entity, ddc::Tag::Visible );

			dd::LightComponent* light = s_world->Access<dd::LightComponent>( entity );
			light->LightType = dd::LightType::Directional;
			light->Colour = glm::vec3( 1, 1, 1 );
			light->Intensity = 0.7;

			dd::TransformComponent* transform = s_world->Access<dd::TransformComponent>( entity );
			glm::vec3 direction( 0.5, 0.4, -0.3 );
			transform->Transform[ 3 ].xyz = direction;
		}

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
			transform->Transform = glm::translate( glm::vec3( 0, 30, 0 ) ) * 
				glm::rotate( glm::radians( 45.0f ), glm::vec3( 1, 0, 0 ) );
		}
		
		// particle system
		{
			/*ddc::Entity entity = s_world->CreateEntity<dd::ParticleSystemComponent, dd::TransformComponent, dd::BoundsComponent>();
			s_world->AddTag( entity, ddc::Tag::Visible );

			dd::TransformComponent* transform = s_world->Access<dd::TransformComponent>( entity );
			transform->SetLocalPosition( glm::vec3( 10, 60, 10 ) );

			dd::BoundsComponent* bounds = s_world->Access<dd::BoundsComponent>( entity );
			bounds->LocalBox = dd::AABB( glm::vec3( -0.5 ), glm::vec3( 0.5 ) );

			dd::ParticleSystemComponent* particle = s_world->Access<dd::ParticleSystemComponent>( entity );
			particle->Age = 0;
			particle->Lifetime = 1000;*/
		}

		// axes
		{
			{
				ddc::Entity x_entity = s_world->CreateEntity<dd::RayComponent, dd::ColourComponent>();
				s_world->AddTag( x_entity, ddc::Tag::Visible );

				dd::RayComponent* x_ray = s_world->Access<dd::RayComponent>( x_entity );
				x_ray->Ray = dd::Ray( glm::vec3( -50, 0, 0 ), glm::vec3( 1, 0, 0 ) );
				x_ray->Length = 100;

				dd::ColourComponent* x_colour = s_world->Access<dd::ColourComponent>( x_entity );
				x_colour->Colour = glm::vec4( 1, 0, 0, 1 );
			}

			{
				ddc::Entity y_entity = s_world->CreateEntity<dd::RayComponent, dd::ColourComponent>();
				s_world->AddTag( y_entity, ddc::Tag::Visible );

				dd::RayComponent* y_ray = s_world->Access<dd::RayComponent>( y_entity );
				y_ray->Ray = dd::Ray( glm::vec3( 0, -50, 0 ), glm::vec3( 0, 1, 0 ) );
				y_ray->Length = 100;

				dd::ColourComponent* y_colour = s_world->Access<dd::ColourComponent>( y_entity );
				y_colour->Colour = glm::vec4( 0, 1, 0, 1 );
			}

			{
				ddc::Entity z_entity = s_world->CreateEntity<dd::RayComponent, dd::ColourComponent>();
				s_world->AddTag( z_entity, ddc::Tag::Visible );

				dd::RayComponent* z_ray = s_world->Access<dd::RayComponent>( z_entity );
				z_ray->Ray = dd::Ray( glm::vec3( 0, 0, -50 ), glm::vec3( 0, 0, 1 ) );
				z_ray->Length = 100;

				dd::ColourComponent* z_colour = s_world->Access<dd::ColourComponent>( z_entity );
				z_colour->Colour = glm::vec4( 0, 0, 1, 1 );
			}
		}

		// bounds
		{
			//ddr::MeshHandle unitCube = ddr::Mesh::Find( "cube" );

			//CreateMeshEntity( *s_world, unitCube, glm::vec4( 1, 1, 1, 1 ), glm::translate( glm::vec3( 10, 60, 10 ) ) );
		}

		// physics
		{
			float sphere_size = 3;
			
			dd::Array<glm::vec3, 2> ball_positions;
			ball_positions.Add( glm::vec3( 0, 60, -30 ) );
			ball_positions.Add( glm::vec3( 0, 60, 30 ) );

			dd::Array<ddc::Entity, 2> balls;
			balls.Add( CreateBall( ball_positions[0], glm::vec4( 0.2, 0.2, 0.8, 1 ), sphere_size ) );
			balls.Add( CreateBall( ball_positions[1], glm::vec4( 0.8, 0.2, 0.8, 1 ), sphere_size ) );

			/*{
				float static_sphere_size = 100;

				{
					glm::mat4 transform = glm::translate( glm::vec3( 0, static_sphere_size * -0.7f, 0 ) ) *
						glm::scale( glm::vec3( static_sphere_size ) );

					ddc::Entity sphere = CreateMeshEntity( *s_world, ddr::Mesh::Find( "sphere" ), glm::vec4( 0.9, 0.9, 0.9, 1 ), transform );
					s_world->AddTag( sphere, ddc::Tag::Static );

					dd::PhysicsSphereComponent& physics_sphere = s_world->Add<dd::PhysicsSphereComponent>( sphere );
					physics_sphere.Sphere = dd::Sphere( glm::vec3( 0, 0, 0 ), 1 );
					physics_sphere.Elasticity = 0.95f;
				}

				{
					glm::mat4 transform = glm::translate( glm::vec3( 0, static_sphere_size * -0.7f, -static_sphere_size * 1.3f ) ) *
						glm::scale( glm::vec3( static_sphere_size ) );

					ddc::Entity sphere = CreateMeshEntity( *s_world, ddr::Mesh::Find( "sphere" ), glm::vec4( 0.9, 0.9, 0.9, 1 ), transform );
					s_world->AddTag( sphere, ddc::Tag::Static );

					dd::PhysicsSphereComponent& physics_sphere = s_world->Add<dd::PhysicsSphereComponent>( sphere );
					physics_sphere.Sphere = dd::Sphere( glm::vec3( 0, 0, 0 ), 1 );
					physics_sphere.Elasticity = 0.95f;
				}

				{
					glm::mat4 transform = glm::translate( glm::vec3( 0, static_sphere_size * -0.7f, static_sphere_size * 1.3f ) ) *
						glm::scale( glm::vec3( static_sphere_size ) );

					ddc::Entity sphere = CreateMeshEntity( *s_world, ddr::Mesh::Find( "sphere" ), glm::vec4( 0.9, 0.9, 0.9, 1 ), transform );
					s_world->AddTag( sphere, ddc::Tag::Static );

					dd::PhysicsSphereComponent& physics_sphere = s_world->Add<dd::PhysicsSphereComponent>( sphere );
					physics_sphere.Sphere = dd::Sphere( glm::vec3( 0, 0, 0 ), 1 );
					physics_sphere.Elasticity = 0.95f;
				}
			}*/

			float plane_size = 100;

			{
				glm::mat4 transform = glm::translate( glm::vec3( 0, 0, 0 ) ) *
					glm::rotate( glm::radians( 45.0f ), glm::vec3( 1, 0, 0 ) ) *
					glm::scale( glm::vec3( plane_size ) );

				ddc::Entity plane = CreateMeshEntity( *s_world, ddr::Mesh::Find( "quad" ), glm::vec4( 0.2, 0.8, 0.2, 1 ), transform );
				s_world->AddTag( plane, ddc::Tag::Static );

				dd::PhysicsPlaneComponent& physics_plane = s_world->Add<dd::PhysicsPlaneComponent>( plane );
				physics_plane.Plane = dd::Plane( glm::vec3( 0, 0, 0 ), glm::vec3( 0, 1, 0 ) );
				physics_plane.Elasticity = 0.95f;
			}

			{
				glm::mat4 transform = glm::translate( glm::vec3( 0, 0, 0 ) ) *
					glm::rotate( glm::radians( -45.0f ), glm::vec3( 1, 0, 0 ) ) *
					glm::scale( glm::vec3( plane_size ) );

				ddc::Entity plane = CreateMeshEntity( *s_world, ddr::Mesh::Find( "quad" ), glm::vec4( 0.8, 0.2, 0.2, 1 ), transform );
				s_world->AddTag( plane, ddc::Tag::Static );

				dd::PhysicsPlaneComponent& physics_plane = s_world->Add<dd::PhysicsPlaneComponent>( plane );
				physics_plane.Plane = dd::Plane( glm::vec3( 0, 0, 0 ), glm::vec3( 0, 1, 0 ) );
				physics_plane.Elasticity = 0.95f;
			}

			ddc::World* world = s_world;
			input_bindings->RegisterHandler( InputAction::RESET_PHYSICS, [balls, ball_positions, world]( InputAction action, InputType type )
			{
				if( type == InputType::RELEASED )
				{
					for( size_t i = 0; i < balls.Size(); ++i )
					{
						dd::TransformComponent* transform = world->Access<dd::TransformComponent>( balls[i] );
						transform->SetPosition( ball_positions[i] );

						dd::PhysicsSphereComponent* sphere = world->Access<dd::PhysicsSphereComponent>( balls[i] );
						sphere->Velocity = glm::vec3( 0, 0, 0 );
						sphere->Resting = false;
					}
				}
			} );
		}

		// everything's set up, so we can start using ImGui - asserts before this will be handled by the default console
		pempek::assert::implementation::setAssertHandler( OnAssert ); 

#ifndef _DEBUG
		::ShowWindow( GetConsoleWindow(), SW_HIDE );
#endif

		while( !s_window->ShouldClose() )
		{
			DD_PROFILE_SCOPED( Frame );

			s_frameTimer->SetMaxFPS( s_maxFPS );
			s_frameTimer->Update();

			float delta_t = s_frameTimer->GameDelta();

			s_input->Update( delta_t );
			s_debugUI->StartFrame( delta_t );

			s_world->Update( delta_t );

			s_fpsCamera->SetAspectRatio( s_window->GetWidth(), s_window->GetHeight() );

			UpdateFreeCam( *s_freeCamera, *s_shakyCamera, *s_input, delta_t );
		
			s_debugUI->RenderDebugPanels( *s_world );
			s_frameTimer->DrawFPSCounter();

			s_renderer->Render( *s_world, *s_shakyCamera );
			
			s_debugUI->EndFrame();
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
	s_scriptEngine = new AngelScriptEngine();
	dd::TypeInfo::SetScriptEngine( s_scriptEngine );
	dd::TypeInfo::RegisterDefaultTypes();

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
