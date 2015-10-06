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
	class DebugUI
	{
	public:
		DebugUI( GLFWwindow* window );
		~DebugUI();

		void Update( float delta_t );

		// Use if you want to reset your rendering device without losing ImGui state.
		void InvalidateDeviceObjects();
		bool CreateDeviceObjects();

		void SetFocused( bool focused );
		void SetMousePosition( float x, float y );
		void SetDisplaySize( int window_width, int window_height );

		static void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
		static void ScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
		static void KeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods );
		static void CharCallback( GLFWwindow* window, unsigned int c );

	private:

		static void RenderDrawLists( ImDrawData* draw_data );
		void CreateFontsTexture();

		bool m_focused;
	};
}