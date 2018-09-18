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

		void StartFrame( float delta_t );
		void EndFrame();

		bool IsMidFrame() const { return m_midFrame; }

		bool CreateDeviceObjects();

		void SetFocused( bool focused );
		void SetMousePosition( glm::vec2 pos );
		void UpdateDisplaySize();

		static void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
		static void ScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
		static void KeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods );
		static void CharCallback( GLFWwindow* window, unsigned int c );

	private:

		static void RenderDrawLists( ImDrawData* draw_data );
		void CreateFontsTexture();

		bool m_midFrame { false };
		bool m_focused { false };
		Input* m_input { nullptr };
		Window* m_window { nullptr };
	};
}