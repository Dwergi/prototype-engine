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

#include "DDAssertHelpers.h"
#include "DebugConsole.h"
#include "DebugUI.h"
#include "EntityPrototype.h"
#include "EntityVisualizer.h"
#include "File.h"
#include "FrameTimer.h"
#include "IGame.h"
#include "IInputSource.h"
#include "InputBindings.h"
#include "InputSystem.h"
#include "JobSystem.h"
#include "Material.h"
#include "Mesh.h"
#include "OpenGL.h"
#include "Services.h"
#include "Texture.h"
#include "Timer.h"
#include "Uniforms.h"
#include "IWindow.h"
#include "SFMLInputSource.h"
#include "SFMLWindow.h"
#include "Sprite.h"
#include "SpriteSheet.h"
#include "World.h"
#include "WorldRenderer.h"

#include <filesystem>
#include <nlohmann/json.hpp>
//---------------------------------------------------------------------------

// GAME TO USE
#include "LuxportGame.h"
using TGame = lux::LuxportGame;
static dd::Service<dd::IGame> s_game;

static dd::Service<dd::IWindow> s_window;
static dd::Service<dd::InputSystem> s_input;
static dd::Service<dd::InputBindings> s_inputBindings;
static dd::Service<dd::IInputSource> s_inputSource;
static dd::Service<dd::DebugUI> s_debugUI;
static dd::Service<ddr::WorldRenderer> s_renderer;
static dd::Service<dd::FrameTimer> s_frameTimer;
static dd::Service<dd::JobSystem> s_jobSystem;
static dd::Service<ddr::ICamera> s_camera;

static ddc::World* g_world;

std::thread::id g_mainThread;

static void ShowSystemConsole(bool show)
{
	::ShowWindow(GetConsoleWindow(), show ? SW_SHOW : SW_HIDE);
}

static void ToggleConsole(dd::InputAction action, dd::InputType type)
{
	if (action == dd::InputAction::TOGGLE_CONSOLE && type == dd::InputType::RELEASED)
	{
		s_debugUI->SetDraw(true);
	}
}

static void ToggleDebugUI(dd::InputAction action, dd::InputType type)
{
	if (action == dd::InputAction::TOGGLE_DEBUG_UI && type == dd::InputType::RELEASED)
	{
		s_input->SetMode(s_input->GetMode() == dd::InputMode::GAME ? dd::InputMode::DEBUG : dd::InputMode::GAME);
	}
}

static void PauseGame(dd::InputAction action, dd::InputType type)
{
	if (action == dd::InputAction::PAUSE && type == dd::InputType::RELEASED)
	{
		s_frameTimer->SetPaused(!s_frameTimer->IsPaused());
	}
}

static void Exit(dd::InputAction action, dd::InputType type)
{
	if (action == dd::InputAction::EXIT && type == dd::InputType::RELEASED)
	{
		s_window->SetToClose();
	}
}

static void SetTimeScale(dd::InputAction action, dd::InputType type)
{
	if (action == dd::InputAction::TIME_SCALE_DOWN && type == dd::InputType::RELEASED)
	{
		float time_scale = s_frameTimer->GetTimeScale();
		s_frameTimer->SetTimeScale(time_scale * 0.9f);
	}

	if (action == dd::InputAction::TIME_SCALE_UP && type == dd::InputType::RELEASED)
	{
		float time_scale = s_frameTimer->GetTimeScale();
		s_frameTimer->SetTimeScale(time_scale * 1.1f);
	}
}

static dd::Service<ddr::TextureManager> s_textureManager;
static dd::Service<ddr::ShaderManager> s_shaderManager;
static dd::Service<ddr::MaterialManager> s_materialManager;
static dd::Service<ddr::MeshManager> s_meshManager;
static dd::Service<ddc::EntityPrototypeManager> s_entityProtoManager;
static dd::Service<ddr::SpriteManager> s_spriteManager;
static dd::Service<ddr::SpriteSheetManager> s_spriteSheetManager;

static void CreateAssetManagers()
{
	DD_TODO("Assets probably shouldn't live in main...");

	dd::Services::Register(new ddr::TextureManager());
	dd::Services::Register(new ddr::ShaderManager());
	dd::Services::Register(new ddr::MaterialManager());
	dd::Services::Register(new ddr::MeshManager());
	dd::Services::Register(new ddc::EntityPrototypeManager());
	dd::Services::Register(new ddr::SpriteManager());
	dd::Services::Register(new ddr::SpriteSheetManager(*s_spriteManager));
}

static void UpdateAssetManagers()
{
	s_textureManager->Update();
	s_shaderManager->Update();
	s_materialManager->Update();
	s_meshManager->Update();
	s_entityProtoManager->Update();
	s_spriteManager->Update();
	s_spriteSheetManager->Update();
}

static int GameMain()
{
	DD_PROFILE_INIT();
	DD_PROFILE_THREAD_NAME("Main");

	dd::TypeInfo::RegisterQueuedTypes();

	dd::Services::Register(new dd::JobSystem(std::thread::hardware_concurrency() - 1));

	dd::Services::RegisterInterface<dd::IGame>(new TGame());

	DD_TODO("Should fetch EntitySpaces from game.");

	{
		dd::IWindow& window = *new dd::SFMLWindow();
		window.SetTitle(s_game->GetTitle())
			.SetSize(glm::ivec2(1024, 768))
			.Initialize();

		OpenGL::Initialize();
		
		dd::Services::RegisterInterface<dd::IWindow>(window);

		dd::Services::RegisterInterface<dd::IInputSource>(new dd::SFMLInputSource());

		dd::Services::Register(new dd::InputBindings());
		//s_inputBindings->RegisterHandler(dd::InputAction::TOGGLE_DEBUG_UI, &ToggleDebugUI);
		s_inputBindings->RegisterHandler(dd::InputAction::EXIT, &Exit);
		s_inputBindings->RegisterHandler(dd::InputAction::PAUSE, &PauseGame);
		s_inputBindings->RegisterHandler(dd::InputAction::TIME_SCALE_DOWN, &SetTimeScale);
		s_inputBindings->RegisterHandler(dd::InputAction::TIME_SCALE_UP, &SetTimeScale);

		dd::Services::Register(new dd::InputSystem());
		s_input->SetBindings(*s_inputBindings);
		s_input->Initialize();
		s_input->SetMode(dd::InputMode::GAME);

		dd::Services::Register(new dd::DebugUI());

		g_world = new ddc::World();
		dd::Services::Register(new ddr::WorldRenderer());

		dd::Services::Register(new dd::FrameTimer());
		s_frameTimer->SetMaxFPS(60);

		dd::EntityVisualizer& entity_visualizer = dd::Services::Register(new dd::EntityVisualizer());
		entity_visualizer.BindActions(*s_inputBindings);

		s_debugUI->RegisterDebugPanel(*s_frameTimer);
		s_debugUI->RegisterDebugPanel(*s_renderer);
		s_debugUI->RegisterDebugPanel(*g_world);
		s_debugUI->RegisterDebugPanel(entity_visualizer);

		CreateAssetManagers();

		s_game->Initialize(*g_world);

		g_world->Initialize();
		s_renderer->InitializeRenderers(*g_world);

		// everything's set up, so we can start using ImGui - asserts before this will be handled by the default console
		dd::InitializeAssert();
		ShowSystemConsole(false);

		while (!s_window->IsClosing())
		{
			DD_PROFILE_SCOPED(Frame);

			dd::Profiler::BeginFrame();

			s_frameTimer->Update();

			s_window->Update(s_frameTimer->AppDelta());
			s_input->Update(s_frameTimer->AppDelta());
			s_debugUI->StartFrame(s_frameTimer->AppDelta());

			g_world->Update(s_frameTimer->GameDelta());

			UpdateAssetManagers();

			s_game->Update(*g_world);

			s_debugUI->RenderDebugPanels(*g_world);
			s_frameTimer->DrawFPSCounter();

			s_game->RenderUpdate(*g_world);

			s_renderer->Render(*g_world, s_camera, s_frameTimer->GameDelta());

			s_debugUI->EndFrame();
			s_window->Swap();

			s_frameTimer->DelayFrame();

			dd::Profiler::EndFrame();
		}
	}

	dd::Services::UnregisterAll();

	DD_PROFILE_DEINIT();

	return 0;
}

#ifdef _TEST

int TestMain(int argc, char* argv[])
{
	dd::TypeInfo::RegisterQueuedTypes();

	int iError = tests::RunTests(argc, argv);

	if (iError != 0)
		DD_ASSERT(false, "Tests failed!");
	else
		printf("Tests passed!");

	return iError;
}

#endif

//
// ENTRY POINT
//
int main(int argc, char* argv[])
{
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());

	dd::CommandLine cmdLine(argv, argc);
	if (cmdLine.Exists("noassert"))
	{
		pempek::assert::implementation::ignoreAllAsserts(true);
	}

	if (cmdLine.Exists("dataroot"))
	{
		dd::File::SetDataRoot(cmdLine.GetValue("dataroot").c_str());
	}
	else
	{
		dd::File::SetDataRoot("../../../data");
	}

#ifdef _TEST
	return TestMain(argc, argv);
#else
	return GameMain();
#endif
}
