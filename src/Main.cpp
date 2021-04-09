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
#include "Services.h"
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

// GAME TO USE
using TGame = flux::FluxGame;
//---------------------------------------------------------------------------

static dd::Service<dd::IGame> s_game;
static dd::Service<dd::IWindow> s_window;
static dd::Service<dd::Input> s_input;
static dd::Service<dd::IInputSource> s_inputSource;
static dd::Service<dd::IDebugUI> s_debugUI;
static dd::Service<ddr::RenderManager> s_renderer;
static dd::Service<dd::FrameTimer> s_frameTimer;
static dd::Service<dd::JobSystem> s_jobSystem;
static dd::Service<ddc::SystemsManager> s_systemsManager;
static dd::Service<dd::AssetManager> s_assetManager;
static dd::Service<dd::CommandLine> s_commandLine;

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
static dd::ProfilerValueRef s_startFrameProfiler("Start Frame");
static dd::ProfilerValueRef s_entityUpdateProfiler("Entity Update");
static dd::ProfilerValueRef s_systemsUpdateProfiler("Systems Update");
static dd::ProfilerValueRef s_gameUpdateProfiler("Game Update");
static dd::ProfilerValueRef s_renderProfiler("Render");
static dd::ProfilerValueRef s_debugUIProfiler("Debug UI");
static dd::ProfilerValueRef s_endFrameProfiler("End Frame");

static int GameMain()
{
	if (s_commandLine->Exists("noassert"))
	{
		ppk::assert::implementation::ignoreAllAsserts(true);
	}

	{
		dd::Services::RegisterInterface<dd::IWindow>(new dd::SFMLWindow())
			.SetSize(glm::ivec2(1024, 768))
			.Initialize();

		OpenGL::Initialize();

		dd::Services::RegisterInterface<dd::IInputSource>(new dd::SFMLInputSource());

		dd::Services::Register(new dd::Input());
		s_input->AddInputSource(*s_inputSource);
		s_input->Initialize();

		s_input->AddHandler(dd::InputAction::TOGGLE_DEBUG_UI, &ToggleDebugUI);
		s_input->AddHandler(dd::InputAction::EXIT, &Exit);
		s_input->AddHandler(dd::InputAction::PAUSE, &PauseGame);
		s_input->AddHandler(dd::InputAction::TIME_SCALE_DOWN, &DecreaseTimeScale);
		s_input->AddHandler(dd::InputAction::TIME_SCALE_UP, &IncreaseTimeScale);

		dd::Services::RegisterInterface<dd::IDebugUI>(new dd::ImGuiDebugUI());

		dd::Services::Register(new ddc::SystemsManager());

		dd::Services::Register(new ddr::RenderManager());

		dd::Services::Register(new dd::FrameTimer());
		s_frameTimer->SetMaxFPS(60);

		dd::Services::Register(new dd::EntityVisualizer());

		dd::Services::Register(new dd::AssetManager());

		std::vector<ddc::EntityLayer*> entity_layers;

		{
			dd::Services::RegisterInterface<dd::IGame>(new TGame());
			s_game->Initialize();
			s_game->RegisterSystems(*s_systemsManager);
			s_game->RegisterRenderers(*s_renderer);
			s_game->CreateEntityLayers(entity_layers);

			for (ddc::EntityLayer* layer : entity_layers)
			{
				s_systemsManager->Initialize(*layer);
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

			s_window->SetTitle(s_game->GetTitle());

			{
				s_profilerTimer.Restart();

				StartFrame();

				s_startFrameProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
			}

			s_assetManager->Update();

			{
				{
					s_profilerTimer.Restart();
					for (ddc::EntityLayer* layer : entity_layers)
					{
						layer->Update();
					}
					s_entityUpdateProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
				}

				{
					s_profilerTimer.Restart();
					for (ddc::EntityLayer* layer : entity_layers)
					{
						s_systemsManager->Update(*layer, s_frameTimer->GameDelta());
					}
					s_systemsUpdateProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
				}

				{
					s_profilerTimer.Restart();
					for (ddc::EntityLayer* layer : entity_layers)
					{
						dd::GameUpdateData update_data(*layer, *s_input, s_frameTimer->GameDelta());
						s_game->Update(update_data);
					}
					s_gameUpdateProfiler.SetValue(s_profilerTimer.TimeInMilliseconds());
				}

				{
					s_profilerTimer.Restart();
					for (ddc::EntityLayer* layer : entity_layers)
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

		for (ddc::EntityLayer* layer : entity_layers)
		{
			s_systemsManager->Shutdown(*layer);

			delete layer;
		}

		entity_layers.clear();

		s_assetManager->Shutdown();
		s_renderer->Shutdown();
		s_game->Shutdown();
	}

	return 0;
}

void Initialize()
{
	dd::InitializeMemoryTracking();
	dd::SetAsMainThread();

	dd::File::SetBasePath("../../../data");
	dd::File::AddOverridePath("./base");

	dd::TypeInfo::RegisterQueuedTypes();

	dd::Services::Register(new dd::JobSystem(std::thread::hardware_concurrency() - 1));
}

void Shutdown()
{
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
