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
#include "IInputSource.h"
#include "IWindow.h"

static dd::Service<dd::IInputSource> s_input;
static dd::Service<dd::IWindow> s_window;

namespace dd
{

	static const char* GetClipboardText( void* data )
	{
		return s_input->GetClipboardText();
	}

	static void SetClipboardText( void* data, const char* text )
	{
		s_input->SetClipboardText(data, text);
	}

	/*void DebugUI::MouseButtonCallback( GLFWwindow*, int button, int action, int / *mods* / )
	{
		if( action == GLFW_PRESS && button >= 0 && button < 3 )
			g_MouseJustPressed[button] = true;
	}

	void DebugUI::ScrollCallback( GLFWwindow*, double / *xoffset* /, double yoffset )
	{
		g_MouseWheel += (float) yoffset; // Use fractional mouse wheel, 1.0 unit 5 lines.
	}

	void DebugUI::KeyCallback( GLFWwindow*, int key, int, int action, int mods )
	{
		ImGuiIO& io = ImGui::GetIO();
		if( action == GLFW_PRESS )
			io.KeysDown[key] = true;
		if( action == GLFW_RELEASE )
			io.KeysDown[key] = false;

		(void) mods; // Modifiers are not reliable across systems
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	}

	void DebugUI::CharCallback( GLFWwindow*, unsigned int c )
	{
		ImGuiIO& io = ImGui::GetIO();
		if( c > 0 && c < 0x10000 )
			io.AddInputCharacter( (unsigned short) c );
	}*/

	DebugUI::DebugUI()
	{
		ImGuiIO& io = ImGui::GetIO();
		/*GLFWInputSource& glfwInput = (GLFWInputSource&) input;

		glfwInput.AddKeyboardCallback( &DebugUI::KeyCallback );
		glfwInput.AddScrollCallback( &DebugUI::ScrollCallback );
		glfwInput.AddMouseCallback( &DebugUI::MouseButtonCallback );
		glfwInput.AddCharCallback( &DebugUI::CharCallback );

		g_Window = window.GetInternalWindow();

		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;*/

		File file = File( "fonts\\Roboto-Medium.ttf" );

		size_t size = file.Size();
		Buffer<byte> buffer(new byte[size], size);
		size_t read = file.Read( buffer );

		io.Fonts->AddFontFromMemoryTTF( buffer.Access(), (int) read, 16.0f );

		//io.RenderDrawListsFn = &DebugUI::RenderDrawLists; // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
		io.SetClipboardTextFn = SetClipboardText;
		io.GetClipboardTextFn = GetClipboardText;

#ifdef _WIN32
		//io.ImeWindowHandle = glfwGetWin32Window( g_Window );
#endif

		//CreateDeviceObjects();
	}

	DebugUI::~DebugUI()
	{
	}

	void DebugUI::SetFocused( bool focused )
	{
		m_focused = focused;
	}

	void DebugUI::SetMousePosition( glm::vec2 pos )
	{
		if( m_focused )
			ImGui::GetIO().MousePos = ImVec2( pos.x, pos.y );   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
		else 
			ImGui::GetIO().MousePos = ImVec2( -1, -1 );
	}
 
	void DebugUI::StartFrame( float delta_t )
	{
		DD_PROFILE_START( DebugUI_Update );

		ImGuiIO& io = ImGui::GetIO();

		if( !s_input->IsMouseCaptured() )
		{
			SetMousePosition( s_input->GetMousePosition().Absolute );
		}

		SetFocused( s_window->IsFocused() );

		// Setup time step
		/*io.DeltaTime = g_Time > 0.0 ? (float) delta_t : (float) (1.0f / 60.0f);
		g_Time += delta_t;

		for( int i = 0; i < 3; i++ )
		{
			io.MouseDown[i] = g_MouseJustPressed[i] || glfwGetMouseButton( g_Window, i ) != 0;    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
			g_MouseJustPressed[i] = false;
		}

		io.MouseWheel = g_MouseWheel;
		g_MouseWheel = 0.0f;*/

		// Start the frame
		ImGui::NewFrame();

		DD_PROFILE_END();

		m_midFrame = true;
	}

	void DebugUI::EndFrame()
	{
		ImGui::Render();

		m_midFrame = false;
	}

	void DebugUI::RegisterDebugPanel( IDebugPanel& debug_panel )
	{
		m_debugPanels.push_back( &debug_panel );

		std::sort( m_debugPanels.begin(), m_debugPanels.end(),
			[]( const IDebugPanel* a, const IDebugPanel* b )
			{
				return strcmp( a->GetDebugTitle(), b->GetDebugTitle() ) < 0;
			}
		);
	}

	void DebugUI::RenderDebugPanels( ddc::World& world )
	{
		if( !m_draw )
			return;

		if( ImGui::BeginMainMenuBar() )
		{
			if( ImGui::BeginMenu( "File" ) )
			{
				if( ImGui::MenuItem( "Exit" ) )
				{
					s_window->SetToClose();
				}

				ImGui::EndMenu();
			}

			if( ImGui::BeginMenu( "Views" ) )
			{
				for( IDebugPanel* debug_view : m_debugPanels )
				{
					debug_view->AddToMenu();
				}

				ImGui::EndMenu();
			}

			if( ImGui::BeginMenu( "Profiler" ) )
			{
				bool draw = dd::Profiler::ShouldDraw();
				if( ImGui::MenuItem( "Show", nullptr, &draw ) )
				{
					dd::Profiler::EnableDraw( draw );
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if( dd::Profiler::ShouldDraw() )
		{
			dd::Profiler::Draw();
		}

		for( IDebugPanel* panel : m_debugPanels )
		{
			if( panel->IsDebugPanelOpen() )
			{
				m_midWindow = true;

				panel->DrawDebugPanel( world );

				m_midWindow = false;
			}
		}
	}
}
