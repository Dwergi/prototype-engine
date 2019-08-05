#include "PCH.h"
#include "NeutrinoGame.h"

#include "BoundsRenderer.h"
#include "BulletSystem.h"
#include "DebugUI.h"
#include "FPSCameraComponent.h"
#include "FrameTimer.h"
#include "FreeCameraController.h"
#include "HitTestSystem.h"
#include "Input.h"
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
#include "Services.h"
#include "WaterRenderer.h"
#include "WaterSystem.h"
#include "IWindow.h"
#include "WorldRenderer.h"

namespace neutrino
{
	static dd::Service<dd::InputKeyBindings> s_inputBindings;
	static dd::Service<dd::Input> s_input;
	//static dd::Service<dd::ShipSystem> s_shipSystem;
	static dd::Service<dd::FreeCameraController> s_freeCamera;
	static dd::Service<dd::FPSCameraComponent> s_player;
	static dd::Service<dd::ShakyCamera> s_shakyCamera;
	static dd::Service<ddr::WorldRenderer> s_renderer;
	static dd::Service<dd::IWindow> s_window;
	static dd::Service<dd::IInputSource> s_inputSource;
	static dd::Service<dd::DebugUI> s_debugUI;
	static dd::Service<dd::JobSystem> s_jobSystem;
	static dd::Service<dd::FrameTimer> s_frameTimer;
	static dd::Service<ddc::SystemManager> s_systemManager;

	static void SetCameraPos(dd::InputAction action, dd::InputType type)
	{
		if (type != dd::InputType::Release)
			return;

		s_player->SetRotation(0, 0);

		const glm::vec3 cube_pos(10.5, 60.5, 10);

		switch (action)
		{
			case dd::InputAction::CAMERA_POS_1:
				s_player->SetPosition(cube_pos - glm::vec3(0, 0, 0.5));
				break;

			case dd::InputAction::CAMERA_POS_2:
				s_player->SetPosition(cube_pos - glm::vec3(0, 0, 1));
				break;

			case dd::InputAction::CAMERA_POS_3:
				s_player->SetPosition(cube_pos - glm::vec3(0, 0, 2));
				break;

			case dd::InputAction::CAMERA_POS_4:
				s_player->SetPosition(cube_pos - glm::vec3(0, 0, 3));
				break;

			case dd::InputAction::DECREASE_DEPTH:
				s_player->SetPosition(s_player->GetPosition() + glm::vec3(0, 0, 1));
				break;

			case dd::InputAction::INCREASE_DEPTH:
				s_player->SetPosition(s_player->GetPosition() - glm::vec3(0, 0, 1));
				break;
		}
	}

	static void UpdateFreeCam(dd::FreeCameraController& free_cam, dd::ShakyCamera& shaky_cam, dd::Input& input, float delta_t)
	{
		if (s_freeCamera->IsEnabled() && s_input->GetMode() == dd::InputMode::GAME)
		{
			free_cam.UpdateMouse(s_inputSource->GetMousePosition());
			free_cam.UpdateScroll(s_inputSource->GetMouseScroll());
		}

		shaky_cam.Update(delta_t);
	}

	static void ToggleFreeCam(dd::InputAction action, dd::InputType type)
	{
		if (action == dd::InputAction::TOGGLE_FREECAM && type == dd::InputType::Release)
		{
			s_freeCamera->Enable(!s_freeCamera->IsEnabled());
			//s_shipSystem->Enable(!s_shipSystem->IsEnabled());
		}
	}

	static void CreateEntities(ddc::EntitySpace& entities)
	{
		// player
		{
			ddc::Entity entity = entities.CreateEntity<dd::TransformComponent, dd::PlayerComponent, dd::FPSCameraComponent>();

			dd::Services::Register(entities.Access<dd::FPSCameraComponent>(entity));
			s_player->SetSize(s_window->GetSize());
			s_player->SetVerticalFOV(glm::radians(45.0f));

			DD_TODO("Shaky camera should be a component/system pair.");
			dd::Services::Register(new dd::ShakyCamera(s_player, s_inputBindings));
			dd::Services::RegisterInterface<ddr::ICamera>(s_shakyCamera.Get());

			s_debugUI->RegisterDebugPanel(s_shakyCamera);

			dd::TransformComponent* transform = entities.Access<dd::TransformComponent>(entity);

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
			light->LightType = dd::LightType::Point;
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

	void NeutrinoGame::Initialize(ddc::EntitySpace& entities)
	{
		dd::SwarmSystem& swarm_system = dd::Services::Register(new dd::SwarmSystem());

		//TrenchSystem trench_system( *s_shakyCam  );
		//trench_system.CreateRenderResources();

		dd::Services::Register(new dd::FreeCameraController());
		s_freeCamera->BindActions(*s_inputBindings);

		dd::HitTestSystem* hit_testing_system = new dd::HitTestSystem();
		dd::Services::RegisterInterface<dd::IAsyncHitTest>(hit_testing_system);
		hit_testing_system->DependsOn(*s_freeCamera);

		dd::PhysicsSystem& physics_system = dd::Services::Register(new dd::PhysicsSystem());

		//Services::Register(new ShipSystem( *s_shakyCam  ));
		//s_shipSystem->BindActions( bindings );
		//s_shipSystem->CreateShip( *s_world );

		dd::TerrainSystem& terrain_system = dd::Services::Register(new dd::TerrainSystem());

		dd::BulletSystem& bullet_system = dd::Services::Register(new dd::BulletSystem());
		bullet_system.DependsOn(*s_freeCamera);
		bullet_system.DependsOn(*hit_testing_system);
		bullet_system.BindActions(*s_inputBindings);

		dd::ParticleSystem& particle_system = dd::Services::Register(new dd::ParticleSystem());
		particle_system.BindActions(*s_inputBindings);

		dd::TreeSystem& tree_system = dd::Services::Register(new dd::TreeSystem());

		dd::WaterSystem& water_system = dd::Services::Register(new dd::WaterSystem(terrain_system.GetTerrainParameters()));
		water_system.DependsOn(terrain_system);

		s_systemManager->Register(*s_freeCamera);
		s_systemManager->Register(terrain_system);
		s_systemManager->Register(particle_system);
		s_systemManager->Register(*hit_testing_system);
		s_systemManager->Register(bullet_system);
		s_systemManager->Register(physics_system);
		s_systemManager->Register(swarm_system);
		s_systemManager->Register(tree_system);
		s_systemManager->Register(water_system);

		DD_TODO("Fix input bindings");
		/*s_inputBindings->RegisterHandler(dd::InputAction::TOGGLE_FREECAM, &ToggleFreeCam);
		s_inputBindings->RegisterHandler(dd::InputAction::CAMERA_POS_1, &SetCameraPos);
		s_inputBindings->RegisterHandler(dd::InputAction::CAMERA_POS_2, &SetCameraPos);
		s_inputBindings->RegisterHandler(dd::InputAction::CAMERA_POS_3, &SetCameraPos);
		s_inputBindings->RegisterHandler(dd::InputAction::CAMERA_POS_4, &SetCameraPos);
		s_inputBindings->RegisterHandler(dd::InputAction::INCREASE_DEPTH, &SetCameraPos);
		s_inputBindings->RegisterHandler(dd::InputAction::DECREASE_DEPTH, &SetCameraPos);*/

		ddr::ParticleSystemRenderer& particle_renderer = dd::Services::Register(new ddr::ParticleSystemRenderer());

		dd::MousePicking& mouse_picking = dd::Services::Register(new dd::MousePicking());
		mouse_picking.BindActions(*s_inputBindings);

		ddr::MeshRenderer& mesh_renderer = dd::Services::Register(new ddr::MeshRenderer());
		ddr::LightRenderer& light_renderer = dd::Services::Register(new ddr::LightRenderer());

		ddr::BoundsRenderer& bounds_renderer = dd::Services::Register(new ddr::BoundsRenderer());
		bounds_renderer.BindKeys(*s_inputBindings);

		ddr::RayRenderer& ray_renderer = dd::Services::Register(new ddr::RayRenderer());

		ddr::LinesRenderer& lines_renderer = dd::Services::Register(new ddr::LinesRenderer());

		ddr::TerrainRenderer& terrain_renderer = dd::Services::Register(new ddr::TerrainRenderer(terrain_system.GetTerrainParameters()));

		ddr::WaterRenderer& water_renderer = dd::Services::Register(new ddr::WaterRenderer());

		s_renderer->Register(mouse_picking);
		s_renderer->Register(light_renderer);
		s_renderer->Register(terrain_renderer);
		s_renderer->Register(particle_renderer);
		s_renderer->Register(mesh_renderer);
		s_renderer->Register(bounds_renderer);
		s_renderer->Register(ray_renderer);
		s_renderer->Register(lines_renderer);
		s_renderer->Register(water_renderer);

		s_debugUI->RegisterDebugPanel(*s_freeCamera);
		s_debugUI->RegisterDebugPanel(mouse_picking);
		s_debugUI->RegisterDebugPanel(particle_system);
		s_debugUI->RegisterDebugPanel(terrain_system);
		s_debugUI->RegisterDebugPanel(mesh_renderer);
		s_debugUI->RegisterDebugPanel(bounds_renderer);
		s_debugUI->RegisterDebugPanel(light_renderer);
		s_debugUI->RegisterDebugPanel(*hit_testing_system);
		s_debugUI->RegisterDebugPanel(bullet_system);
		s_debugUI->RegisterDebugPanel(physics_system);
		s_debugUI->RegisterDebugPanel(terrain_renderer);
		s_debugUI->RegisterDebugPanel(swarm_system);
		s_debugUI->RegisterDebugPanel(tree_system);
		s_debugUI->RegisterDebugPanel(water_system);

		CreateEntities(entities);
	}

	void NeutrinoGame::Update(ddc::EntitySpace& entities)
	{
	}

	void NeutrinoGame::RenderUpdate(ddc::EntitySpace& entities)
	{
		s_player->SetSize(s_window->GetSize());

		UpdateFreeCam(s_freeCamera, s_shakyCamera, s_input, s_frameTimer->AppDelta());
	}

	void NeutrinoGame::Shutdown(ddc::EntitySpace& entities)
	{

	}
}