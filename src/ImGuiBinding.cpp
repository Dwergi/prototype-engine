#include "PCH.h"
#include "ImGuiBinding.h"

#include "Input.h"
#include "IInputSource.h"
#include "IWindow.h"

#include <sfml/Window.hpp>
#include <imgui/imgui_impl_opengl3.h>

namespace dd
{
	static dd::Service<IWindow> s_window;
	static dd::Service<IInputSource> s_inputSource;
	static dd::Service<Input> s_input;

	static bool s_handleMouse = false;

	static double g_Time = 0.0;
	static dd::Cursor g_MouseCursors[ImGuiMouseCursor_COUNT] = { dd::Cursor::Hidden };

	static int g_click[5];

	void ImGuiBinding::SetMouseHandling(bool handle)
	{
		s_handleMouse = true;
	}

	bool ImGuiBinding::IsMouseHandled()
	{
		return s_handleMouse;
	}

	void ImGuiBinding::Initialize()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext(); 
		
		ImGuiIO& io = ImGui::GetIO();

		g_Time = 0.0;

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)

		io.BackendPlatformName = "dd_sfml";

		// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
		io.KeyMap[ImGuiKey_Tab] = (int) Key::TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = (int) Key::LEFT_ARROW;
		io.KeyMap[ImGuiKey_RightArrow] = (int) Key::RIGHT_ARROW;
		io.KeyMap[ImGuiKey_UpArrow] = (int) Key::UP_ARROW;
		io.KeyMap[ImGuiKey_DownArrow] = (int) Key::DOWN_ARROW;
		io.KeyMap[ImGuiKey_PageUp] = (int) Key::PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = (int) Key::PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = (int) Key::HOME;
		io.KeyMap[ImGuiKey_End] = (int) Key::END;
		io.KeyMap[ImGuiKey_Insert] = (int) Key::INSERT;
		io.KeyMap[ImGuiKey_Delete] = (int) Key::DEL;
		io.KeyMap[ImGuiKey_Backspace] = (int) Key::BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = (int) Key::SPACE;
		io.KeyMap[ImGuiKey_Enter] = (int) Key::ENTER;
		io.KeyMap[ImGuiKey_Escape] = (int) Key::ESCAPE;
		io.KeyMap[ImGuiKey_A] = (int) Key::A;
		io.KeyMap[ImGuiKey_C] = (int) Key::C;
		io.KeyMap[ImGuiKey_V] = (int) Key::V;
		io.KeyMap[ImGuiKey_X] = (int) Key::X;
		io.KeyMap[ImGuiKey_Y] = (int) Key::Y;
		io.KeyMap[ImGuiKey_Z] = (int) Key::Z;

		sf::Window* sfml_window = (sf::Window*) s_window->GetNative();
		io.ImeWindowHandle = (void*) sfml_window->getSystemHandle();

		g_MouseCursors[ImGuiMouseCursor_Arrow] = dd::Cursor::Arrow;
		g_MouseCursors[ImGuiMouseCursor_TextInput] = dd::Cursor::Text;
		g_MouseCursors[ImGuiMouseCursor_ResizeAll] = dd::Cursor::SizeAll;
		g_MouseCursors[ImGuiMouseCursor_ResizeNS] = dd::Cursor::SizeNS;
		g_MouseCursors[ImGuiMouseCursor_ResizeEW] = dd::Cursor::SizeEW;
		g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = dd::Cursor::SizeNESW;
		g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = dd::Cursor::SizeNWSE;
		g_MouseCursors[ImGuiMouseCursor_Hand] = dd::Cursor::Hand;

		ImGui::StyleColorsDark();

		ImGui_ImplOpenGL3_Init(nullptr);
	}

	void ImGuiBinding::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();

		ImGui::DestroyContext();
	}

	static void UpdateMouse()
	{
		ImGuiIO& io = ImGui::GetIO();
		
		if (!s_handleMouse)
		{
			io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX); 
			io.MouseWheel = 0;
			io.MouseWheelH = 0;
			return;
		}

		for (int i = 0; i < 5; ++i)
		{
			if (g_click[i] >= 2)
			{
				io.MouseDown[i] = false;
			}
			g_click[i] = 0;
		}

		// Update buttons
		dd::Array<InputEvent, 64> events;
		s_inputSource->GetEvents(events);

		for (const InputEvent& evt : events)
		{
			if (!evt.IsMouse())
			{
				continue;
			}

			int index = (int) evt.Key - (int) Key::MOUSE_LEFT;
			
			if (evt.Type == InputType::Press)
			{
				io.MouseDown[index] = true;

				++g_click[index];
			}

			if (evt.Type == InputType::Release)
			{
				io.MouseDown[index] = false;

				if (g_click[index] > 0)
				{
					++g_click[index];
				}
			}
		}

		// handle clicks as MouseDown
		for (int i = 0; i < 5; ++i)
		{
			if (g_click[i] >= 2)
			{
				io.MouseDown[i] = true;
			}
		}

		// Update mouse position
		const ImVec2 mouse_pos_backup = io.MousePos;
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
		const bool focused = s_window->IsFocused();
		
		if (focused)
		{
			if (io.WantSetMousePos)
			{
				s_inputSource->SetMousePosition(glm::vec2(mouse_pos_backup.x, mouse_pos_backup.y));
			}
			else
			{
				dd::MousePosition mouse_pos = s_inputSource->GetMousePosition();
				io.MousePos = ImVec2(mouse_pos.Absolute.x, mouse_pos.Absolute.y);
			}
		}

		// Update mouse scroll
		MousePosition scroll_pos = s_inputSource->GetMouseScroll();
		io.MouseWheel = scroll_pos.Absolute.y;
		io.MouseWheelH = scroll_pos.Absolute.x;
	}

	static void UpdateMouseCursor()
	{
		ImGuiIO& io = ImGui::GetIO();
		if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || s_window->GetCursor() == dd::Cursor::Hidden)
		{
			return;
		}

		ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
		if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			s_window->SetCursor(dd::Cursor::Hidden);
		}
		else
		{
			// Show OS mouse cursor
			s_window->SetCursor(g_MouseCursors[imgui_cursor] != dd::Cursor::Hidden ? g_MouseCursors[imgui_cursor] : dd::Cursor::Arrow);
		}
	}

	static ImGuiKey_ GetImGuiKey(dd::Key key)
	{
		switch (key)
		{
		case Key::TAB:			return ImGuiKey_Tab;
		case Key::LEFT_ARROW:	return ImGuiKey_LeftArrow;
		case Key::RIGHT_ARROW:	return ImGuiKey_RightArrow;
		case Key::UP_ARROW:		return ImGuiKey_UpArrow;
		case Key::DOWN_ARROW:	return ImGuiKey_DownArrow;
		case Key::PAGE_UP:		return ImGuiKey_PageUp;
		case Key::PAGE_DOWN:	return ImGuiKey_PageDown;
		case Key::HOME:			return ImGuiKey_Home;
		case Key::END:			return ImGuiKey_End;
		case Key::INSERT:		return ImGuiKey_Insert;
		case Key::DEL:			return ImGuiKey_Delete;
		case Key::BACKSPACE:	return ImGuiKey_Backspace;
		case Key::SPACE:		return ImGuiKey_Space;
		case Key::ENTER:		return ImGuiKey_Enter;
		case Key::ESCAPE:		return ImGuiKey_Escape;
		case Key::A:			return ImGuiKey_A;
		case Key::C:			return ImGuiKey_C;
		case Key::V:			return ImGuiKey_V;
		case Key::X:			return ImGuiKey_X;
		case Key::Y:			return ImGuiKey_Y;
		case Key::Z:			return ImGuiKey_Z;
		}

		return ImGuiKey_COUNT;
	}

	static void UpdateKeyboard()
	{
		ImGuiIO& io = ImGui::GetIO();

		dd::Array<InputEvent, 64> events;
		s_inputSource->GetEvents(events);

		for (const InputEvent& evt : events)
		{
			if (evt.IsMouse())
			{
				continue;
			}

			ImGuiKey_ imkey = GetImGuiKey(evt.Key);
			if (imkey >= ImGuiKey_COUNT)
			{
				continue;
			}

			if (evt.Type == InputType::Press)
			{
				io.KeysDown[imkey] = true;

				if (evt.Modifiers.Has(Modifier::Alt))
				{
					io.KeyAlt = true;
				}
				if (evt.Modifiers.Has(Modifier::Shift))
				{
					io.KeyShift = true;
				}
				if (evt.Modifiers.Has(Modifier::Ctrl))
				{
					io.KeyCtrl = true;
				}
			}

			if (evt.Type == InputType::Release)
			{
				io.KeysDown[imkey] = false;
			}
		}
	}

	static void UpdateWindowSize()
	{
		ImGuiIO& io = ImGui::GetIO();
		
		glm::ivec2 cur_size = s_window->GetSize();
		
		io.DisplaySize = ImVec2(float(cur_size.x), float(cur_size.y));
		io.DisplayFramebufferScale = ImVec2(1, 1);
	}

	void ImGuiBinding::StartFrame(float delta_t)
	{
		ImGuiIO& io = ImGui::GetIO();

		UpdateWindowSize();

		double current_time = g_Time + delta_t;
		io.DeltaTime = g_Time > 0.0 ? ( float) (current_time - g_Time) : ( float) (1.0f / 60.0f);
		g_Time = current_time;

		ImGui_ImplOpenGL3_NewFrame();

		UpdateMouse();
		UpdateMouseCursor();

		UpdateKeyboard();

		s_input->EnableKeyboard(!io.WantCaptureKeyboard);
		s_input->EnableKeyboard(!io.WantCaptureMouse);

		ImGui::NewFrame();
	}

	void ImGuiBinding::EndFrame()
	{
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}