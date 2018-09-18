//
// DebugUI.h - ImGui binding.
// Copyright (C) Sebastian Nordgren 
// October 6th 2015
//

#pragma once

struct GLFWwindow;
struct ImDrawData;

namespace ddc
{
	struct World;
}

namespace dd
{
	class IDebugPanel;
	class Input;
	class Window;

	class DebugUI
	{
	public:
		DebugUI( Window& window, Input& input );
		~DebugUI();

		void StartFrame( float delta_t );
		void EndFrame();

		bool Draw() const { return m_draw; }
		void EnableDraw( bool enable ) { m_draw = enable; }

		bool IsMidFrame() const { return m_midFrame; }
		bool IsMidWindow() const { return m_midWindow; }

		bool CreateDeviceObjects();

		void SetFocused( bool focused );
		void SetMousePosition( glm::vec2 pos );
		void UpdateDisplaySize();

		void RegisterDebugPanel( IDebugPanel& debug_panel );
		void RenderDebugPanels( const ddc::World& world );

		static void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
		static void ScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
		static void KeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods );
		static void CharCallback( GLFWwindow* window, unsigned int c );

	private:

		static void RenderDrawLists( ImDrawData* draw_data );
		void CreateFontsTexture();

		std::vector<IDebugPanel*> m_debugPanels;

		bool m_draw { false };
		bool m_midWindow { false };
		bool m_midFrame { false };
		bool m_focused { false };
		Input* m_input { nullptr };
		Window* m_window { nullptr };
	};
}