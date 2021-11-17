//
// NeutrinoGame.cpp
// Copyright (C) Sebastian Nordgren 
// August 21st 2019
//

#include "PCH.h"
#include "NeutrinoGame.h"

#include "BoundsRenderer.h"
#include "BulletSystem.h"
#include "DebugUI.h"
#include "File.h"
#include "FPSCameraComponent.h"
#include "FrameTimer.h"
#include "FreeCameraController.h"
#include "HitTestSystem.h"
#include "Input.h"
#include "InputKeyBindings.h"
#include "IWindow.h"
#include "LightComponent.h"
#include "LightRenderer.h"
#include "LinesRenderer.h"
#include "MeshRenderer.h"
#include "MousePicking.h"
#include "ParticleSystem.h"
#include "ParticleSystemRenderer.h"
#include "PlayerComponent.h"
#include "PhysicsSystem.h"
#include "RayRenderer.h"
#include "ShakyCamera.h"
#include "ShipSystem.h"
#include "SwarmAgentComponent.h"
#include "SwarmSystem.h"
#include "TerrainRenderer.h"
#include "TerrainSystem.h"
#include "TestEntities.h"
#include "TransformComponent.h"
#include "TreeSystem.h"
#include "TrenchSystem.h"
#include "WaterRenderer.h"
#include "WaterSystem.h"

#include "ddc/SystemsManager.h"

#include "ddr/RenderManager.h"

namespace neut
{
	static dd::Service<dd::Input> s_input;
	static dd::Service<dd::InputKeyBindings> s_keybindings;
	//static dd::Service<dd::ShipSystem> s_shipSystem;
	static dd::Service<dd::FreeCameraController> s_freeCamera;
	//static dd::Service<dd::ShakyCamera> s_shakyCamera;
	static dd::Service<dd::IWindow> s_window;
	static dd::Service<dd::IInputSource> s_inputSource;
	static dd::Service<dd::IDebugUI> s_debugUI;
	static dd::Service<dd::JobSystem> s_jobSystem;
	static dd::Service<dd::FrameTimer> s_frameTimer;
	static dd::Service<neut::TerrainSystem> s_terrain;
	static dd::Service<ddr::RenderManager> s_renderer;

	static ddc::Entity s_player;

	enum class CameraPos
	{
		One,
		Two,
		Three,
		Four,
		IncreaseDepth,
		DecreaseDepth
	};

	static void SetCameraPos(CameraPos pos)
	{
		DD_ASSERT(s_player.IsAlive());

		dd::FPSCameraComponent* camera = s_player.Access<dd::FPSCameraComponent>();
		DD_ASSERT(camera != nullptr);

		camera->SetRotation(0, 0);

		const glm::vec3 cube_pos(10.5, 60.5, 10);

		switch (pos)
		{
		case CameraPos::One:
			camera->SetPosition(cube_pos - glm::vec3(0, 0, 0.5));
				break;

			case CameraPos::Two:
				camera->SetPosition(cube_pos - glm::vec3(0, 0, 1));
				break;

			case CameraPos::Three:
				camera->SetPosition(cube_pos - glm::vec3(0, 0, 2));
				break;

			case CameraPos::Four:
				camera->SetPosition(cube_pos - glm::vec3(0, 0, 3));
				break;

			case CameraPos::IncreaseDepth:
				camera->SetPosition(camera->GetPosition() + glm::vec3(0, 0, 1));
				break;

			case CameraPos::DecreaseDepth:
				camera->SetPosition(camera->GetPosition() - glm::vec3(0, 0, 1));
				break;
		}
	}

	static void UpdateShakyCam(dd::FreeCameraController& free_cam, dd::ShakyCamera& shaky_cam, dd::Input& input, float delta_t)
	{
		shaky_cam.Update(delta_t);
	}

	static void ToggleFreeCam()
	{
		s_freeCamera->SetEnabled(!s_freeCamera->IsEnabled());
		//s_shipSystem->SetEnabled(!s_shipSystem->IsEnabled());
	}

	static void CreateEntities(ddc::EntityLayer& entities)
	{
		// player
		{
			s_player = entities.CreateEntity<dd::TransformComponent, dd::PlayerComponent, dd::FPSCameraComponent>();

			dd::FPSCameraComponent* camera = s_player.Access<dd::FPSCameraComponent>();
			camera->SetAspectRatio(s_window->GetAspectRatio());
			camera->SetVerticalFOV(glm::radians(45.0f));

			s_renderer->SetCamera(*camera);
		}

		// dir light
		{
			ddc::Entity entity = entities.CreateEntity<dd::LightComponent, dd::TransformComponent>();
			entities.AddTag(entity, ddc::Tag::Visible);

			dd::LightComponent* light = entities.Access<dd::LightComponent>(entity);
			light->LightType = dd::LightType::Directional;
			light->Colour = glm::vec3(1, 1, 1);
			light->Intensity = 0.7;

			dd::TransformComponent* transform = entities.Access<dd::TransformComponent>(entity);
			transform->Rotation = glm::angleAxis(glm::radians(45.0f), glm::vec3(1, 1, 0));
			transform->Update();
		}

		// point light
		{
			ddc::Entity entity = entities.CreateEntity<dd::LightComponent, dd::TransformComponent>();
			entities.AddTag(entity, ddc::Tag::Visible);

			dd::LightComponent* light = entities.Access<dd::LightComponent>(entity);
			light->LightType = dd::LightType::Spot;
			light->Colour = glm::vec3(1, 1, 1);
			light->Intensity = 3;
			light->Ambient = 0.01;
			light->Attenuation = 0.03;
			light->InnerAngle = glm::radians(30.f);
			light->OuterAngle = glm::radians(45.f);

			dd::TransformComponent* transform = entities.Access<dd::TransformComponent>(entity);
			transform->Position = glm::vec3(0, 30, 0);
			transform->Rotation = glm::angleAxis(glm::radians(45.0f), glm::vec3(1, 0, 0));
			transform->Update();
		}

		// particle system
		/*{
			ddc::Entity entity = entities.CreateEntity<dd::ParticleSystemComponent, dd::TransformComponent, dd::BoundBoxComponent>();
			entities.AddTag( entity, ddc::Tag::Visible );
			entities.AddTag( entity, ddc::Tag::Dynamic );

			dd::TransformComponent* transform = entities.Access<dd::TransformComponent>( entity );
			transform->Position = glm::vec3( 10, 60, 10 );
			transform->Update();

			dd::BoundBoxComponent* bounds = entities.Access<dd::BoundBoxComponent>( entity );
			bounds->BoundBox = ddm::AABB( glm::vec3( -0.5 ), glm::vec3( 0.5 ) );

			dd::ParticleSystemComponent* particle = entities.Access<dd::ParticleSystemComponent>( entity );
			particle->Age = 0;
			particle->Lifetime = 1000;
		}*/

		// axes
		dd::TestEntities::CreateAxes(entities);

		// physics
		//dd::TestEntities::CreatePhysicsPlaneTestScene();
	}

	void NeutrinoGame::Initialize()
	{
		dd::File::AddOverridePath("./neutrino");

		dd::InputModeConfig::Create("game")
			.ShowCursor(false)
			.CaptureMouse(true)
			.CentreMouse(true);

		s_keybindings->BindKey(dd::Key::F2, dd::InputAction::TOGGLE_FREECAM, "game");
		s_keybindings->BindKey(dd::Key::F2, dd::InputAction::TOGGLE_CONSOLE, "debug");
		s_keybindings->BindKey(dd::Key::F3, dd::InputAction::TOGGLE_BOUNDS);
		s_keybindings->BindKey(dd::Key::F4, dd::InputAction::TOGGLE_PICKING, "debug");
		s_keybindings->BindKey(dd::Key::W, dd::InputAction::FORWARD);
		s_keybindings->BindKey(dd::Key::S, dd::InputAction::BACKWARD);
		s_keybindings->BindKey(dd::Key::A, dd::InputAction::LEFT);
		s_keybindings->BindKey(dd::Key::D, dd::InputAction::RIGHT);
		s_keybindings->BindKey(dd::Key::SPACE, dd::InputAction::UP);
		s_keybindings->BindKey(dd::Key::R, dd::InputAction::ADD_MINOR_TRAUMA);
		s_keybindings->BindKey(dd::Key::T, dd::InputAction::ADD_MAJOR_TRAUMA);
		s_keybindings->BindKey(dd::Key::LCTRL, dd::InputAction::DOWN);
		s_keybindings->BindKey(dd::Key::LSHIFT, dd::InputAction::BOOST);
		s_keybindings->BindKey(dd::Key::KEY_1, dd::InputAction::CAMERA_POS_1);
		s_keybindings->BindKey(dd::Key::KEY_2, dd::InputAction::CAMERA_POS_2);
		s_keybindings->BindKey(dd::Key::KEY_3, dd::InputAction::CAMERA_POS_3);
		s_keybindings->BindKey(dd::Key::KEY_4, dd::InputAction::CAMERA_POS_4);
		s_keybindings->BindKey(dd::Key::HOME, dd::InputAction::DECREASE_DEPTH);
		s_keybindings->BindKey(dd::Key::END, dd::InputAction::INCREASE_DEPTH);
		s_keybindings->BindKey(dd::Key::PAGE_DOWN, dd::InputAction::TIME_SCALE_DOWN);
		s_keybindings->BindKey(dd::Key::PAGE_UP, dd::InputAction::TIME_SCALE_UP);
		s_keybindings->BindKey(dd::Key::E, dd::InputAction::TOGGLE_ENTITY_DATA, "debug");
		s_keybindings->BindKey(dd::Key::MOUSE_LEFT, dd::InputAction::SELECT_MESH, "debug");
		s_keybindings->BindKey(dd::Key::MOUSE_LEFT, dd::InputAction::SHOOT, "game");
		s_keybindings->BindKey(dd::Key::H, dd::InputAction::RESET_PHYSICS, "game");

		s_input->AddHandler(dd::InputAction::TOGGLE_FREECAM, &ToggleFreeCam);
		s_input->AddHandler(dd::InputAction::CAMERA_POS_1, []() { SetCameraPos(CameraPos::One); });
		s_input->AddHandler(dd::InputAction::CAMERA_POS_2, []() { SetCameraPos(CameraPos::Two); });
		s_input->AddHandler(dd::InputAction::CAMERA_POS_3, []() { SetCameraPos(CameraPos::Three); });
		s_input->AddHandler(dd::InputAction::CAMERA_POS_4, []() { SetCameraPos(CameraPos::Four); });
		s_input->AddHandler(dd::InputAction::INCREASE_DEPTH, []() { SetCameraPos(CameraPos::IncreaseDepth); });
		s_input->AddHandler(dd::InputAction::DECREASE_DEPTH, []() { SetCameraPos(CameraPos::DecreaseDepth); });

		s_input->SetCurrentMode("game");
	}

	void NeutrinoGame::Update(const dd::GameUpdateData& update_data)
	{
		dd::FPSCameraComponent* camera = s_player.Access<dd::FPSCameraComponent>();
		camera->SetAspectRatio(s_window->GetAspectRatio());

		s_renderer->SetCamera(*camera);
		//UpdateShakyCam(s_freeCamera, camera, s_input, s_frameTimer->AppDelta());
	}

	void NeutrinoGame::Shutdown()
	{

	}

	void NeutrinoGame::CreateEntityLayers(std::vector<ddc::EntityLayer*>& entity_layers)
	{
		entity_layers.push_back(new ddc::EntityLayer("game"));

		CreateEntities(*entity_layers[0]);
	}

	void NeutrinoGame::RegisterRenderers(ddr::RenderManager& renderer)
	{
		ddr::LightRenderer& light_renderer = dd::Services::Register(new ddr::LightRenderer());

		neut::TerrainRenderer& terrain_renderer = dd::Services::Register(new neut::TerrainRenderer(s_terrain->GetTerrainParameters()));

		ddr::ParticleSystemRenderer& particle_renderer = dd::Services::Register(new ddr::ParticleSystemRenderer());

		ddr::MeshRenderer& mesh_renderer = dd::Services::Register(new ddr::MeshRenderer());
		
		ddr::BoundsRenderer& bounds_renderer = dd::Services::Register(new ddr::BoundsRenderer());

		ddr::RayRenderer& ray_renderer = dd::Services::Register(new ddr::RayRenderer());

		ddr::LinesRenderer& lines_renderer = dd::Services::Register(new ddr::LinesRenderer());

		neut::WaterRenderer& water_renderer = dd::Services::Register(new neut::WaterRenderer());

		dd::Service<dd::MousePicking> mouse_picking;
		renderer.Register(*mouse_picking);
		renderer.Register(light_renderer);
		renderer.Register(terrain_renderer);
		renderer.Register(particle_renderer);
		renderer.Register(mesh_renderer);
		renderer.Register(bounds_renderer);
		renderer.Register(ray_renderer);
		renderer.Register(lines_renderer);
		renderer.Register(water_renderer);
	}

	void NeutrinoGame::RegisterSystems(ddc::SystemsManager& system_manager)
	{
		dd::Services::Register(new dd::FreeCameraController());

		dd::MousePicking& mouse_picking = dd::Services::Register(new dd::MousePicking());

		neut::SwarmSystem& swarm_system = dd::Services::Register(new neut::SwarmSystem());

		//neut::TrenchSystem trench_system( *s_shakyCamera  );
		//trench_system.CreateRenderResources();

		dd::HitTestSystem* hit_testing_system = new dd::HitTestSystem();
		dd::Services::RegisterInterface<dd::IAsyncHitTest>(hit_testing_system);
		hit_testing_system->DependsOn(*s_freeCamera);

		dd::PhysicsSystem& physics_system = dd::Services::Register(new dd::PhysicsSystem());

		//Services::Register(new ShipSystem( *s_shakyCamera  ));
		//s_shipSystem->BindActions( bindings );
		//s_shipSystem->CreateShip( *s_world );

		neut::TerrainSystem& terrain_system = dd::Services::Register(new neut::TerrainSystem());

		neut::BulletSystem& bullet_system = dd::Services::Register(new neut::BulletSystem());
		bullet_system.DependsOn(*s_freeCamera);
		bullet_system.DependsOn(*hit_testing_system);

		dd::ParticleSystem& particle_system = dd::Services::Register(new dd::ParticleSystem());

		neut::TreeSystem& tree_system = dd::Services::Register(new neut::TreeSystem());

		neut::WaterSystem& water_system = dd::Services::Register(new neut::WaterSystem(terrain_system.GetTerrainParameters()));
		water_system.DependsOn(terrain_system);

		system_manager.Register(*s_freeCamera);
		system_manager.Register(terrain_system);
		system_manager.Register(particle_system);
		system_manager.Register(*hit_testing_system);
		system_manager.Register(bullet_system);
		system_manager.Register(physics_system);
		system_manager.Register(swarm_system);
		//system_manager.Register(tree_system);
		system_manager.Register(water_system);
	}
}