//
// DebugUI.cpp - ImGui binding.
// Copyright (C) Sebastian Nordgren 
// October 6th 2015
//

#include "PCH.h"
#include "DebugUI.h"

#include "File.h"
#include "GLError.h"
#include "IDebugPanel.h"
#include "InputMode.h"
#include "InputSystem.h"
#include "ImGuiBinding.h"
#include "IWindow.h"

static dd::Service<dd::InputSystem> s_input;
static dd::Service<dd::IWindow> s_window;
static dd::Service<dd::DebugUI> s_debugUI;

namespace dd
{
	void OnEnterDebugMode()
	{
		s_debugUI->SetMouseHandling(true);
		s_debugUI->SetDraw(true);
	}

	void OnExitDebugMode()
	{
		s_debugUI->SetMouseHandling(false);
		s_debugUI->SetDraw(false);
	}

	DebugUI::DebugUI()
	{
		ImGuiBinding::Initialize();

		dd::InputMode::Access(dd::InputMode::DEBUG)
			.OnEnter(&OnEnterDebugMode)
			.OnExit(&OnExitDebugMode);
	}

	DebugUI::~DebugUI()
	{
		ImGuiBinding::Shutdown();
	}

	DebugUI& DebugUI::SetMouseHandling(bool handle)
	{
		ImGuiBinding::SetMouseHandling(handle);
		return *this;
	}

	bool DebugUI::IsMouseHandled() const
	{
		return ImGuiBinding::IsMouseHandled();
	}

	void DebugUI::StartFrame(float delta_t)
	{
		DD_PROFILE_SCOPED(DebugUI_Update);

		m_midFrame = true;

		ImGuiBinding::StartFrame(delta_t);
	}

	void DebugUI::EndFrame()
	{
		ImGuiBinding::EndFrame();

		m_midFrame = false;
	}

	void DebugUI::RegisterDebugPanel(IDebugPanel& debug_panel)
	{
		m_debugPanels.push_back(&debug_panel);

		std::sort(m_debugPanels.begin(), m_debugPanels.end(),
			[](const IDebugPanel* a, const IDebugPanel* b)
		{
			return strcmp(a->GetDebugTitle(), b->GetDebugTitle()) < 0;
		}
		);
	}

	void DebugUI::RenderDebugPanels(ddc::World& world)
	{
		if (!m_draw)
			return;

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					s_window->SetToClose();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Views"))
			{
				for (IDebugPanel* debug_view : m_debugPanels)
				{
					debug_view->AddToMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Profiler"))
			{
				bool draw = dd::Profiler::ShouldDraw();
				if (ImGui::MenuItem("Show", nullptr, &draw))
				{
					dd::Profiler::EnableDraw(draw);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (dd::Profiler::ShouldDraw())
		{
			dd::Profiler::Draw();
		}

		for (IDebugPanel* panel : m_debugPanels)
		{
			if (panel->IsDebugPanelOpen())
			{
				m_midWindow = true;

				panel->DrawDebugPanel(world);

				m_midWindow = false;
			}
		}
	}
}
