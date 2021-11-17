//
// Main.cpp - Entry point into application.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#include "PCH.h"

#ifdef _TEST

#include "Tests.h"

#endif

#include "AssetManager.h"
#include "CommandLine.h"
#include "DDAssertHelpers.h"
#include "DebugConsole.h"
#include "DebugUI.h"
#include "EntityVisualizer.h"
#include "File.h"
#include "FrameTimer.h"
#include "IGame.h"
#include "IInputSource.h"
#include "InputKeyBindings.h"
#include "Input.h"
#include "JobSystem.h"
#include "Timer.h"
#include "IWindow.h"
#include "SFMLInputSource.h"
#include "SFMLWindow.h"

#include "ddc/SystemsManager.h"

#include "ddr/OpenGL.h"
#include "ddr/RenderManager.h"
#include "ddr/Uniforms.h"

#include "nlohmann/json.hpp"

#include <filesystem>

//---------------------------------------------------------------------------
#include "stress/StressTestGame.h"
#include "lux/LuxportGame.h"
#include "neutrino/NeutrinoGame.h"
#include "flux/FluxGame.h"
#include "phys2d/Physics2DGame.h"

// GAME TO USE
using TGame = neut::NeutrinoGame;
//using TGame = flux::FluxGame;
//using TGame = phys2d::Physics2DGame;
//---------------------------------------------------------------------------

static dd::Service<dd::IGame> s_game;
static dd::Service<dd::IWindow> s_window;
static dd::Service<dd::Input> s_input;
static dd::Service<dd::IInputSource> s_inputSource;
static dd::Service<dd::InputKeyBindings> s_keybindings;
static dd::Service<dd::IDebugUI> s_debugUI;
static dd::Service<ddr::RenderManager> s_renderer;
static dd::Service<dd::FrameTimer> s_frameTimer;
static dd::Service<dd::JobSystem> s_jobSystem;
static dd::Service<ddc::SystemsManager> s_systemsManager;
static dd::Service<dd::AssetManager> s_assetManager;
static dd::Service<dd::CommandLine> s_commandLine;

std::thread::id g_mainThread;

std::vector<ddc::EntityLayer*> g_entityLayers;

static void ShowSystemConsole(bool show)
{
	::ShowWindow(GetConsoleWindow(), show ? SW_SHOW : SW_HIDE);
}

static void ToggleDebugUI()
{
	bool is_toggled = s_input->GetCurrentMode() == "debug";
	s_debugUI->SetDraw(!is_toggled);
	s_input->SetCurrentMode(is_toggled ? "game" : "debug");
}

static void ToggleDebugUIMouseCapture()
{
	s_input->SetCurrentMode(s_input->GetCurrentMode() == "game" ? "debug" : "game");
}

static void ToggleProfiler()
{
	dd::Profiler::EnableDraw(!dd::Profiler::ShouldDraw());
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

static dd::Timer s_profilerTimer;

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

#if 0
#define DD_BREAK_ON_ALLOC(Frame) if (dd::Profiler::FrameCount() == (Frame)) { dd::BreakOnAlloc(true); }
#else
#define DD_BREAK_ON_ALLOC(Frame)
#endif

// Profiler values
static dd::ProfilerValueRef s_entityUpdateProfiler("Update/Entity");
static dd::ProfilerValueRef s_systemsUpdateProfiler("Update/Systems");
static dd::ProfilerValueRef s_gameUpdateProfiler("Update/Game");
static dd::ProfilerValueRef s_startFrameProfiler("Render/Start Frame");
static dd::ProfilerValueRef s_renderProfiler("Render/Render");
static dd::ProfilerValueRef s_debugUIProfiler("Render/Debug UI");
static dd::ProfilerValueRef s_endFrameProfiler("Render/End Frame");

static void InitializeGame()
{
	if (s_commandLine->Exists("noassert"))
	{
		ppk::assert::implementation::ignoreAllAsserts(true);
	}

	dd::Services::RegisterInterface<dd::IWindow>(new dd::SFMLWindow())
		.SetSize(glm::ivec2(1920, 1080))
		.Initialize();

	OpenGL::Initialize();

	dd::Services::RegisterInterface<dd::IInputSource>(new dd::SFMLInputSource());

	dd::Services::Register(new dd::Input());
	s_input->AddInputSource(*s_inputSource);
	s_input->Initialize();

	s_input->AddHandler(dd::InputAction::TOGGLE_DEBUG_UI, &ToggleDebugUI);
	s_input->AddHandler(dd::InputAction::TOGGLE_DEBUG_UI_CAPTURE, &ToggleDebugUIMouseCapture);
	s_input->AddHandler(dd::InputAction::EXIT, &Exit);
	s_input->AddHandler(dd::InputAction::PAUSE, &PauseGame);
	s_input->AddHandler(dd::InputAction::TIME_SCALE_DOWN, &DecreaseTimeScale);
	s_input->AddHandler(dd::InputAction::TIME_SCALE_UP, &IncreaseTimeScale);
	s_input->AddHandler(dd::InputAction::TOGGLE_PROFILER, &ToggleProfiler);

	dd::Services::Register(new dd::InputKeyBindings());

	s_keybindings->BindKey(dd::Key::ESCAPE, dd::InputAction::TOGGLE_DEBUG_UI);
	s_keybindings->BindKey(dd::Key::ESCAPE, dd::Modifier::Shift, dd::InputAction::TOGGLE_DEBUG_UI_CAPTURE);
	s_keybindings->BindKey(dd::Key::P, dd::InputAction::PAUSE);
	s_keybindings->BindKey(dd::Key::O, dd::InputAction::TOGGLE_PROFILER);

	dd::Services::RegisterInterface<dd::IDebugUI>(new dd::ImGuiDebugUI());

	dd::Services::Register(new ddc::SystemsManager());

	dd::Services::Register(new dd::AssetManager());

	dd::Services::Register(new ddr::RenderManager());

	dd::Services::Register(new dd::FrameTimer());
	s_frameTimer->SetMaxFPS(60);

	dd::Services::Register(new dd::EntityVisualizer());

	{
		dd::Services::RegisterInterface<dd::IGame>(new TGame());
		s_game->Initialize();
		s_game->RegisterSystems(*s_systemsManager);
		s_game->RegisterRenderers(*s_renderer);
		s_game->CreateEntityLayers(g_entityLayers);

		for (ddc::EntityLayer* layer : g_entityLayers)
		{
			s_systemsManager->Initialize(*layer);
		}

		s_renderer->Initialize();
	}

	// everything is set up, so we can start using ImGui - asserts before this will be handled by the default console
	dd::Assert::Initialize();
	ShowSystemConsole(false);
}

static void GameShutdown()
{
	for (ddc::EntityLayer* layer : g_entityLayers)
	{
		s_systemsManager->Shutdown(*layer);

		delete layer;
	}

	g_entityLayers.clear();

	s_renderer->Shutdown();
	s_game->Shutdown();
}

static int GameMain()
{
	InitializeGame();

	while (!s_window->IsClosing())
	{
		DD_PROFILE_SCOPED(Frame);
		DD_BREAK_ON_ALLOC(100);

		s_window->SetTitle(s_game->GetTitle());

		{
			s_profilerTimer.Restart();

			StartFrame();

			s_startFrameProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
		}

		{
			{
				s_profilerTimer.Restart();
				for (ddc::EntityLayer* layer : g_entityLayers)
				{
					layer->Update();
				}
				s_entityUpdateProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
			}

			{
				s_profilerTimer.Restart();
				for (ddc::EntityLayer* layer : g_entityLayers)
				{
					s_systemsManager->Update(*layer, s_frameTimer->GameDelta());
				}
				s_systemsUpdateProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
			}

			{
				s_profilerTimer.Restart();
				for (ddc::EntityLayer* layer : g_entityLayers)
				{
					dd::GameUpdateData update_data(*layer, *s_input, s_frameTimer->GameDelta());
					s_game->Update(update_data);
				}
				s_gameUpdateProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
			}

			s_assetManager->Update();

			{
				s_profilerTimer.Restart();
				for (ddc::EntityLayer* layer : g_entityLayers)
				{
					s_renderer->Render(*layer, s_frameTimer->GameDelta());
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

	GameShutdown();

	return 0;
}

static void Initialize()
{
	dd::InitializeMemoryTracking();
	dd::SetAsMainThread();

	dd::File::SetBasePath("../../../data");
	dd::File::AddOverridePath("./base");

	dd::TypeInfo::RegisterQueuedTypes();

	dd::Services::Register(new dd::JobSystem(std::thread::hardware_concurrency() - 1));
}

static void Shutdown()
{
	dd::Assert::Shutdown();
	dd::Profiler::Shutdown();
	dd::Services::UnregisterAll();
}

//
// ENTRY POINT
//
#ifndef _TEST

int main(int argc, char* argv[])
{
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());
	dd::Services::Register(new dd::CommandLine(argv, argc));

	Initialize();

	int32 ret = GameMain();

	Shutdown();

	return ret;
}
#endif // !_TEST
