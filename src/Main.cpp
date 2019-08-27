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
#include "SystemsManager.h"
#include "RenderManager.h"

#include <filesystem>
#include <nlohmann/json.hpp>

//---------------------------------------------------------------------------
// GAME TO USE
#include "stress/StressTestGame.h"
using TGame = stress::StressTestGame;
//#include "lux/LuxportGame.h"
//using TGame = lux::LuxportGame;
//#include "neutrino/NeutrinoGame.h"
//using TGame = neut::NeutrinoGame;
//---------------------------------------------------------------------------

static dd::Service<dd::IGame> s_game;
static dd::Service<dd::IWindow> s_window;
static dd::Service<dd::Input> s_input;
static dd::Service<dd::IInputSource> s_inputSource;
static dd::Service<dd::DebugUI> s_debugUI;
static dd::Service<ddr::RenderManager> s_renderer;
static dd::Service<dd::FrameTimer> s_frameTimer;
static dd::Service<dd::JobSystem> s_jobSystem;
static dd::Service<ddc::SystemsManager> s_systemsManager;

static ddc::EntityLayer* g_tempSpace;

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

static dd::Timer s_profilerTimer;

static void StartFrame()
{
	DD_DIAGNOSTIC("-------------------- START FRAME --------------------\n");

	dd::Profiler::BeginFrame();

	s_frameTimer->Update();

	s_window->Update(s_frameTimer->AppDelta());
	s_input->Update(s_frameTimer->AppDelta());

	s_debugUI->StartFrame(s_frameTimer->AppDelta());
}

static void EndFrame()
{
	s_jobSystem->Clear();

	s_debugUI->EndFrame();
	s_window->Swap();

	s_frameTimer->DelayFrame();

	dd::Profiler::EndFrame();
}

#if 0
#define DD_BREAK_ON_ALLOC(Frame) if (dd::Profiler::FrameCount() == (Frame)) { dd::BreakOnAlloc(true); }
#else
#define DD_BREAK_ON_ALLOC(Frame)
#endif

// Profiler values
static dd::ProfilerValue& s_startFrameProfiler = dd::Profiler::GetValue("Start Frame");
static dd::ProfilerValue& s_entityUpdateProfiler = dd::Profiler::GetValue("Entity Update");
static dd::ProfilerValue& s_systemsUpdateProfiler = dd::Profiler::GetValue("Systems Update");
static dd::ProfilerValue& s_gameUpdateProfiler = dd::Profiler::GetValue("Game Update");
static dd::ProfilerValue& s_renderProfiler = dd::Profiler::GetValue("Render");
static dd::ProfilerValue& s_debugUIProfiler = dd::Profiler::GetValue("Debug UI");
static dd::ProfilerValue& s_endFrameProfiler = dd::Profiler::GetValue("End Frame");

static int GameMain()
{
	DD_PROFILE_INIT();
	DD_PROFILE_THREAD_NAME("Main");

	dd::InitializeMemoryTracking();
	dd::SetAsMainThread();

	dd::TypeInfo::RegisterQueuedTypes();

	dd::Services::Register(new dd::JobSystem(std::thread::hardware_concurrency()));

	dd::Services::RegisterInterface<dd::IGame>(new TGame());

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

		dd::Services::Register(new ddc::SystemsManager());

		dd::Services::Register(new ddr::RenderManager());

		dd::Services::Register(new dd::FrameTimer());
		s_frameTimer->SetMaxFPS(60);

		dd::EntityVisualizer& entity_visualizer = dd::Services::Register(new dd::EntityVisualizer());

		s_debugUI->RegisterDebugPanel(*s_frameTimer);
		s_debugUI->RegisterDebugPanel(*s_renderer);
		s_debugUI->RegisterDebugPanel(*s_systemsManager);
		s_debugUI->RegisterDebugPanel(entity_visualizer);

		CreateAssetManagers();

		std::vector<ddc::EntityLayer*> entity_layers;

		{
			s_game->Initialize();
			s_game->RegisterSystems(*s_systemsManager);
			s_game->RegisterRenderers(*s_renderer);
			s_game->CreateEntityLayers(entity_layers);

			for (ddc::EntityLayer* space : entity_layers)
			{
				s_systemsManager->Initialize(*space);
			}

			s_renderer->Initialize();
		}

		// everything is set up, so we can start using ImGui - asserts before this will be handled by the default console
		dd::InitializeAssert();
		ShowSystemConsole(false);

		while (!s_window->IsClosing())
		{
			DD_PROFILE_SCOPED(Frame);
			DD_BREAK_ON_ALLOC(100);

			{
				s_profilerTimer.Restart();

				StartFrame();

				s_startFrameProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
			}

			UpdateAssetManagers();

			{
				{
					s_profilerTimer.Restart();
					for (ddc::EntityLayer* space : entity_layers)
					{
						space->Update(s_frameTimer->GameDelta());
					}
					s_entityUpdateProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
				}

				{
					s_profilerTimer.Restart();
					for (ddc::EntityLayer* space : entity_layers)
					{
						s_systemsManager->Update(*space, s_frameTimer->GameDelta());
					}
					s_systemsUpdateProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
				}

				{
					s_profilerTimer.Restart();
					for (ddc::EntityLayer* space : entity_layers)
					{
						dd::GameUpdateData update_data(*space, *s_input, s_frameTimer->GameDelta());
						s_game->Update(update_data);
					}
					s_gameUpdateProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
				}

				{
					s_profilerTimer.Restart();
					for (ddc::EntityLayer* space : entity_layers)
					{
						s_renderer->Render(*space, s_game->GetCamera(), s_frameTimer->GameDelta());
					}
					s_renderProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
				}
			}

			{
				s_profilerTimer.Restart();

				s_frameTimer->DrawFPSCounter();
				s_debugUI->RenderDebugPanels();
				dd::Profiler::Draw();

				s_debugUIProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
			}

			{
				s_profilerTimer.Restart();

				EndFrame();

				s_endFrameProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
			}

			dd::BreakOnAlloc(false);
		}

		for (ddc::EntityLayer* space : entity_layers)
		{
			s_systemsManager->Shutdown(*space);

			delete space;
		}

		entity_layers.clear();

		s_renderer->Shutdown();
		s_game->Shutdown();
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
