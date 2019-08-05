//
// DebugUI.h - ImGui binding.
// Copyright (C) Sebastian Nordgren 
// October 6th 2015
//

#pragma once

struct ImDrawData;
struct ImGuiContext;

namespace ddc
{
	struct EntitySpace;
}

namespace dd
{
	struct IDebugPanel;
	struct IInputSource;
	struct Window;

	struct DebugUI
	{
		DebugUI();
		~DebugUI();

		void StartFrame( float delta_t );
		void EndFrame();

		bool ShouldDraw() const { return m_draw; }
		DebugUI& SetDraw(bool draw) { m_draw = draw; return *this; }

		bool IsMidFrame() const { return m_midFrame; }
		bool IsMidWindow() const { return m_midWindow; }

		void RegisterDebugPanel( IDebugPanel& debug_panel );
		void RenderDebugPanels( ddc::EntitySpace& entities );

	private:
		std::vector<IDebugPanel*> m_debugPanels;

		bool m_captureMouse { false };
		bool m_draw { false };
		bool m_midWindow { false };
		bool m_midFrame { false };

		ImGuiContext* m_imguiContext { nullptr };
	};
}