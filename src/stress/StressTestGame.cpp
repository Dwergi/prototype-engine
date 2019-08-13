//
// StressTestGame.cpp - Game file for engine stress test.
// Copyright (C) Sebastian Nordgren 
// August 9th 2019
//

#pragma once

#include "PCH.h"
#include "stress/StressTestGame.h"

#include "DebugUI.h"
#include "FreeCameraController.h"
#include "Input.h"
#include "InputKeyBindings.h"
#include "LightComponent.h"
#include "LightRenderer.h"
#include "MeshRenderer.h"
#include "Random.h"
#include "RayRenderer.h"
#include "RenderManager.h"
#include "TestEntities.h"
#include "IWindow.h"
#include "SystemsManager.h"

#include "ColourComponent.h"
#include "FPSCameraComponent.h"
#include "MeshComponent.h"
#include "RayComponent.h"
#include "TransformComponent.h"

namespace stress
{
	static dd::Service<dd::Input> s_input;
	static dd::Service<dd::DebugUI> s_debugUI;
	static dd::Service<dd::IWindow> s_window;
	static dd::Service<dd::FreeCameraController> s_freeCam;

	static dd::InputKeyBindings* s_keybindings;

	static ddr::MeshHandle s_unitCube;

	static ddc::Entity s_camera;

	static void ToggleProfiler()
	{
		dd::Profiler::EnableDraw(!dd::Profiler::ShouldDraw());
	}

	void StressTestGame::Initialize()
	{
		dd::InputModeConfig& game_input = dd::InputModeConfig::Create("game");
		game_input.ShowCursor(false)
			.CaptureMouse(true)
			.CentreMouse(true);

		s_input->SetCurrentMode("game");

		s_input->AddHandler(dd::InputAction::TOGGLE_PROFILER, &ToggleProfiler);

		s_keybindings = new dd::InputKeyBindings("stress_test");
		s_keybindings->BindKey(dd::Key::ESCAPE, dd::InputAction::TOGGLE_DEBUG_UI);
		s_keybindings->BindKey(dd::Key::P, dd::InputAction::TOGGLE_PROFILER);
		s_keybindings->BindKey(dd::Key::W, dd::InputAction::FORWARD);
		s_keybindings->BindKey(dd::Key::S, dd::InputAction::BACKWARD);
		s_keybindings->BindKey(dd::Key::A, dd::InputAction::LEFT);
		s_keybindings->BindKey(dd::Key::D, dd::InputAction::RIGHT);
		s_keybindings->BindKey(dd::Key::SPACE, dd::InputAction::UP);
		s_keybindings->BindKey(dd::Key::LCTRL, dd::InputAction::DOWN);
		s_keybindings->BindKey(dd::Key::LSHIFT, dd::InputAction::BOOST);

		s_input->SetKeyBindings(*s_keybindings);

		s_debugUI->RegisterDebugPanel(*this);
	}

	void StressTestGame::Shutdown()
	{

	}

	static uint s_entitiesStart = 0;

	static void DestroyEntities(ddc::EntitySpace& space, size_t count)
	{
		if (count == 0)
		{
			return;
		}

		dd::Random32 rng(s_entitiesStart, space.Size() - 1);

		// destroy entities
		while (count > 0)
		{
			ddc::Entity entity = space.GetEntity(rng.Next());
			if (entity.IsAlive())
			{
				space.DestroyEntity(entity);
				--count;
			}
		}
	}

	static const int MAX_POSITION = 1000;

	static void CreateEntities(ddc::EntitySpace& space, uint count)
	{
		if (count == 0)
		{
			return;
		}

		dd::RandomFloat rng(0, 1);

		while (count > 0)
		{
			ddc::Entity entity = space.CreateEntity<dd::TransformComponent, dd::MeshComponent, dd::ColourComponent>();
			entity.AddTag(ddc::Tag::Visible);

			dd::TransformComponent* transform = entity.Access<dd::TransformComponent>();
			transform->Position = glm::vec3(rng.Next() * MAX_POSITION, rng.Next() * MAX_POSITION, rng.Next() * MAX_POSITION);
			transform->Update();

			dd::ColourComponent* colour = entity.Access<dd::ColourComponent>();
			colour->Colour = glm::vec4(rng.Next(), rng.Next(), rng.Next(), 1);

			dd::MeshComponent* mesh = entity.Access<dd::MeshComponent>();
			mesh->Mesh = s_unitCube;

			--count;
		}
	}

	static bool s_first = true;

	void StressTestGame::Update(const dd::GameUpdateData& update_data)
	{
		ddc::EntitySpace& space = update_data.EntitySpace();
		
		s_freeCam->SetEnabled(s_input->GetCurrentMode() == "game");

		int entity_count = space.LiveCount();

		if (s_first)
		{
			dd::Service<ddr::MeshManager> mesh_manager;
			s_unitCube = mesh_manager->Find("cube");

			s_entitiesStart = entity_count;
			s_first = false;
		}

		if (entity_count > m_entityCount)
		{
			uint to_destroy = entity_count - m_entityCount;
			DestroyEntities(space, to_destroy);
		}
		else if (entity_count < m_entityCount)
		{
			// create entities
			uint to_create = (uint) (m_entityCount - entity_count);
			CreateEntities(space, to_create);
		}

		CreateEntities(space, m_createCount);
		DestroyEntities(space, m_createCount);
	}

	void StressTestGame::CreateEntitySpaces(std::vector<ddc::EntitySpace*>& entity_spaces)
	{
		entity_spaces.push_back(new ddc::EntitySpace("game"));

		s_camera = entity_spaces[0]->CreateEntity<dd::TransformComponent, dd::FPSCameraComponent>();

		dd::FPSCameraComponent* camera = s_camera.Access<dd::FPSCameraComponent>();
		camera->SetWindowSize(s_window->GetSize());
		camera->SetVerticalFOV(glm::radians(45.0f));

		glm::vec3 cam_pos(MAX_POSITION, MAX_POSITION, MAX_POSITION);
		camera->SetPosition(cam_pos);
		camera->SetDirection(glm::vec3(0) - cam_pos);

		dd::TestEntities::CreateAxes(*entity_spaces[0]);

		// dir light
		{
			ddc::Entity entity = entity_spaces[0]->CreateEntity<dd::LightComponent, dd::TransformComponent>();
			entity.AddTag(ddc::Tag::Visible);

			dd::LightComponent* light = entity.Access<dd::LightComponent>();
			light->LightType = dd::LightType::Directional;
			light->Colour = glm::vec3(1, 1, 1);
			light->Intensity = 0.7;

			dd::TransformComponent* transform = entity.Access<dd::TransformComponent>();
			transform->Rotation = glm::angleAxis(glm::radians(45.0f), glm::vec3(1, 1, 0));
			transform->Update();
		}
	}

	void StressTestGame::DrawDebugInternal()
	{
		ImGui::SliderInt("Entity Count", &m_entityCount, 0, 1024 * 1024);
		ImGui::SliderInt("Create/Destroy Count", &m_createCount, 0, 1024 * 1024);
	}
	
	void StressTestGame::RegisterRenderers(ddr::RenderManager& render_manager)
	{
		ddr::LightRenderer* light_renderer = new ddr::LightRenderer();
		s_debugUI->RegisterDebugPanel(*light_renderer);

		ddr::MeshRenderer* mesh_renderer = new ddr::MeshRenderer();
		s_debugUI->RegisterDebugPanel(*mesh_renderer);

		ddr::RayRenderer* ray_renderer = new ddr::RayRenderer();

		render_manager.Register(*light_renderer);
		render_manager.Register(*mesh_renderer);
		render_manager.Register(*ray_renderer);
	}

	void StressTestGame::RegisterSystems(ddc::SystemsManager& systems_manager)
	{
		dd::FreeCameraController& free_cam = dd::Services::Register(new dd::FreeCameraController());
		systems_manager.Register(free_cam);
		s_debugUI->RegisterDebugPanel(free_cam);
	}

	ddr::ICamera& StressTestGame::GetCamera() const
	{
		DD_ASSERT(s_camera.IsAlive());
		return *s_camera.Access<dd::FPSCameraComponent>();
	}
}