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
#include "InputKeyBindings.h"
#include "Input.h"
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
#include "SystemManager.h"
#include "WorldRenderer.h"

#include <filesystem>
#include <nlohmann/json.hpp>

//---------------------------------------------------------------------------
// GAME TO USE
#include "lux/LuxportGame.h"
using TGame = lux::LuxportGame;
static dd::Service<dd::IGame> s_game;
//---------------------------------------------------------------------------

static dd::Service<dd::IWindow> s_window;
static dd::Service<dd::Input> s_input;
static dd::Service<dd::IInputSource> s_inputSource;
static dd::Service<dd::DebugUI> s_debugUI;
static dd::Service<ddr::WorldRenderer> s_renderer;
static dd::Service<dd::FrameTimer> s_frameTimer;
static dd::Service<dd::JobSystem> s_jobSystem;
static dd::Service<ddr::ICamera> s_camera;
static dd::Service<ddc::SystemManager> s_systemManager;

static ddc::EntitySpace* g_tempSpace;

std::thread::id g_mainThread;

static void ShowSystemConsole(bool show)
{
	::ShowWindow(GetConsoleWindow(), show ? SW_SHOW : SW_HIDE);
}

static void ToggleConsole()
{
	s_debugUI->SetDraw(true);
}

static void ToggleDebugUI()
{
	s_input->SetCurrentMode(s_input->GetCurrentMode() == "game" ? "debug" : "game");
}

static void PauseGame()
{
	s_frameTimer->SetPaused(!s_frameTimer->IsPaused());
}

static void Exit()
{
	s_window->SetToClose();
}

static void DecreaseTimeScale()
{
	float time_scale = s_frameTimer->GetTimeScale();
	s_frameTimer->SetTimeScale(time_scale * 0.9f);
}

static void IncreaseTimeScale()
{
	float time_scale = s_frameTimer->GetTimeScale();
	s_frameTimer->SetTimeScale(time_scale * 1.1f);
}

static dd::Service<ddr::TextureManager> s_textureManager;
static dd::Service<ddr::ShaderManager> s_shaderManager;
static dd::Service<ddr::MaterialManager> s_materialManager;
static dd::Service<ddr::MeshManager> s_meshManager;
static dd::Service<ddc::EntityPrototypeManager> s_entityProtoManager;

static void CreateAssetManagers()
{
	DD_TODO("Assets probably shouldn't live in main...");

	dd::Services::Register(new ddr::TextureManager());
	dd::Services::Register(new ddr::ShaderManager());
	dd::Services::Register(new ddr::MaterialManager());
	dd::Services::Register(new ddr::MeshManager());
	dd::Services::Register(new ddc::EntityPrototypeManager());
}

static void UpdateAssetManagers()
{
	s_textureManager->Update();
	s_shaderManager->Update();
	s_materialManager->Update();
	s_meshManager->Update();
	s_entityProtoManager->Update();
}

static void StartFrame()
{
	dd::Profiler::BeginFrame();

	s_frameTimer->Update();

	s_window->Update(s_frameTimer->AppDelta());
	s_input->Update(s_frameTimer->AppDelta());

	s_debugUI->StartFrame(s_frameTimer->AppDelta());
}

static void EndFrame()
{
	s_debugUI->EndFrame();
	s_window->Swap();

	s_frameTimer->DelayFrame();

	dd::Profiler::EndFrame();
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

		dd::Services::Register(new dd::Input());
		s_input->AddInputSource(*s_inputSource);
		s_input->Initialize();

		s_input->AddHandler(dd::InputAction::TOGGLE_DEBUG_UI, &ToggleDebugUI);
		s_input->AddHandler(dd::InputAction::EXIT, &Exit);
		s_input->AddHandler(dd::InputAction::PAUSE, &PauseGame);
		s_input->AddHandler(dd::InputAction::TIME_SCALE_DOWN, &DecreaseTimeScale);
		s_input->AddHandler(dd::InputAction::TIME_SCALE_UP, &IncreaseTimeScale);

		dd::Services::Register(new dd::DebugUI());

		dd::Services::Register(new ddc::SystemManager());

		DD_TODO("IGame should provide EntitySpace, update all");
		g_tempSpace = new ddc::EntitySpace("default");

		dd::Services::Register(new ddr::WorldRenderer());

		dd::Services::Register(new dd::FrameTimer());
		s_frameTimer->SetMaxFPS(60);

		dd::EntityVisualizer& entity_visualizer = dd::Services::Register(new dd::EntityVisualizer());

		s_debugUI->RegisterDebugPanel(*s_frameTimer);
		s_debugUI->RegisterDebugPanel(*s_renderer);
		s_debugUI->RegisterDebugPanel(*s_systemManager);
		s_debugUI->RegisterDebugPanel(entity_visualizer);

		CreateAssetManagers();

		{
			dd::GameUpdateData initial_update(*g_tempSpace, *s_input, 0);
			s_game->Initialize(initial_update);
		}

		s_systemManager->Initialize(*g_tempSpace);
		s_renderer->InitializeRenderers(*g_tempSpace);

		// everything's set up, so we can start using ImGui - asserts before this will be handled by the default console
		dd::InitializeAssert();
		ShowSystemConsole(false);

		while (!s_window->IsClosing())
		{
			DD_PROFILE_SCOPED(Frame);

			StartFrame();

			UpdateAssetManagers();

			{
				DD_TODO("Add a loop over all available entity spaces.");
				g_tempSpace->Update(s_frameTimer->GameDelta());
				s_systemManager->Update(*g_tempSpace, s_frameTimer->GameDelta());

				dd::GameUpdateData update_data(*g_tempSpace, *s_input, s_frameTimer->GameDelta());
				s_game->Update(update_data);

				s_game->RenderUpdate(update_data);

				s_renderer->Render(*g_tempSpace, s_camera, s_frameTimer->GameDelta());
			}

			s_frameTimer->DrawFPSCounter();
			s_debugUI->RenderDebugPanels(*g_tempSpace);

			EndFrame();
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
