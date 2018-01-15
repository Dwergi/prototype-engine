//
// DebugUI.h - ImGui binding.
// Copyright (C) Sebastian Nordgren 
// October 6th 2015
//

#pragma once

struct GLFWwindow;
struct ImDrawData;

namespace dd
{
	class Input;
	class Window;

	class DebugUI
	{
	public:
		DebugUI( Window& window, Input& input );
		~DebugUI();

		void Update( float delta_t );

		bool CreateDeviceObjects();

		void SetFocused( bool focused );
		void SetMousePosition( glm::vec2 pos );
		void SetDisplaySize( int window_width, int window_height );

		static void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
		static void ScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
		static void KeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods );
		static void CharCallback( GLFWwindow* window, unsigned int c );

	private:

		static void RenderDrawLists( ImDrawData* draw_data );
		void CreateFontsTexture();

		bool m_focused;
		Input* m_input;
		Window* m_window;
	};
}