//
// DDAssert.cpp - Wrappers to display an ImGui assert dialog.
// Copyright (C) Sebastian Nordgren 
// January 14th 2018
//

#include "PCH.h"
#include "DDAssert.h"

#include "DebugUI.h"
#include "FrameTimer.h"
#include "IInputSource.h"
#include "ImGuiBinding.h"
#include "Input.h"
#include "IWindow.h"

#include "ddr/OpenGL.h"

#include "imgui/imgui.h"

namespace dd::Assert
{
	struct Assert
	{
		String256 Info;
		String256 Message;
		bool Open { false };
		AssertAction Action { AssertAction::None };
	};

	static Assert s_assert;

	String256 FormatAssert(int level, const char* file, int line, const char* function, const char* expression)
	{
		String256 out;
		switch (level)
		{
		case (int) AssertLevel::Debug:
			out += "DEBUG";
			break;

		case (int) AssertLevel::Warning:
			out += "WARNING";
			break;

		case (int) AssertLevel::Error:
			out += "ERROR";
			break;

		case (int) AssertLevel::Fatal:
			out += "FATAL";
			break;
		}

		char buffer[1024];
		snprintf(buffer, 1024, ": \"%s\" in %s() (%s:%d)\n", expression, function, file, line);

		out += buffer;

		return out;
	}

	void DrawAssertDialog(glm::ivec2 window_size, Assert& assert)
	{
		ImGui::SetNextWindowPos(ImVec2(window_size.x / 2.0f - window_size.x / 6.5f, window_size.y / 2.0f - window_size.y / 6.5f), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Assert", &assert.Open))
		{
			ImGui::Text(assert.Info.c_str());
			ImGui::Text(assert.Message.c_str());
			ImGui::Spacing();

			if (ImGui::Button("Break"))
			{
				assert.Action = AssertAction::Break;
				assert.Open = false;
			}

			ImGui::SameLine();

			if (ImGui::Button("Ignore"))
			{
				assert.Action = AssertAction::Ignore;
				assert.Open = false;
			}

			ImGui::SameLine();

			if (ImGui::Button("Ignore This"))
			{
				assert.Action = AssertAction::IgnoreLine;
				assert.Open = false;
			}

			ImGui::SameLine();

			if (ImGui::Button("Abort"))
			{
				assert.Action = AssertAction::Abort;
				assert.Open = false;
			}

			ImGui::End();
		}
		else
		{
			assert.Action = AssertAction::Ignore;
		}
	}

	static dd::Service<dd::FrameTimer> s_frameTimer;
	static dd::Service<dd::Input> s_input;
	static dd::Service<dd::IDebugUI> s_debugUI;
	static dd::Service<dd::IWindow> s_window;

	static bool g_initialized = false;

	static ppk::assert::implementation::AssertAction::AssertAction OnAssert(const char* file, int line, const char* function, const char* expression, int level, const char* message)
	{
		s_assert.Info = FormatAssert(level, file, line, function, expression);
		s_assert.Message = String256();
		s_assert.Action = AssertAction::None;

		if (message != nullptr)
		{
			s_assert.Message += "Message: ";
			s_assert.Message += message;
		}

		dd::String256 s_message = s_assert.Info;
		s_message += s_assert.Message;
		s_message += "\n";

		printf(s_message.c_str());
		OutputDebugStringA(s_message.c_str());

		// not ready or already showing an assert, just ignore
		if (!g_initialized || s_assert.Open)
		{
			return ppk::assert::implementation::AssertAction::Ignore;
		}

		s_assert.Open = true;

		do
		{
			if (dd::IsMainThread())
			{
				Check();
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				__debugbreak();
			}
		} while (s_assert.Action == AssertAction::None);

		return (ppk::assert::implementation::AssertAction::AssertAction) s_assert.Action;
	}

	static void OnEnterInputMode()
	{
		ImGuiBinding::SetMouseHandling(true);
	}

	static void OnExitInputMode()
	{
		ImGuiBinding::SetMouseHandling(false);
	}

	void Initialize()
	{
		dd::InputModeConfig::Create("assert")
			.CaptureMouse(false)
			.CentreMouse(false)
			.ShowCursor(true)
			.OnEnter(&OnEnterInputMode)
			.OnExit(&OnExitInputMode);

		ppk::assert::implementation::setAssertHandler(OnAssert);

		g_initialized = true;
	}

	void Shutdown()
	{
		g_initialized = false;
	}

	static void StartFrame()
	{
		dd::Profiler::BeginFrame();

		s_frameTimer->Update();
		float delta_t = s_frameTimer->AppDelta();

		s_window->Update(delta_t);
		s_input->Update(delta_t);

		glClear(GL_COLOR_BUFFER_BIT);

		s_debugUI->StartFrame(delta_t);
	}

	static void EndFrame()
	{
		s_debugUI->EndFrame();
		s_window->Swap();

		s_frameTimer->DelayFrame();

		dd::Profiler::EndFrame();
	}

	void Check()
	{
		if (s_assert.Open)
		{
			std::string previous_input_mode;
			previous_input_mode = s_input->GetCurrentMode();
			s_input->SetCurrentMode("assert");

			bool was_mid_window = s_debugUI->IsMidWindow();
			if (was_mid_window)
			{
				s_debugUI->EndWindow();
			}

			bool was_mid_frame = s_debugUI->IsMidFrame();
			if (was_mid_frame)
			{
				EndFrame();
			}

			static int s_frameCount;
			s_frameCount = 0;
			do
			{
				StartFrame();

				DrawAssertDialog(s_window->GetSize(), s_assert);

				EndFrame();

				++s_frameCount;
			}
			while (s_assert.Open);

			// try to clean up
			if (was_mid_frame)
			{
				StartFrame();
			}

			if (was_mid_window)
			{
				ImGui::Begin("post_assert");
			}

			s_input->SetCurrentMode(previous_input_mode);
		}
	}
}