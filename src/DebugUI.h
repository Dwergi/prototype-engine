//
// DebugUI.h - ImGui binding.
// Copyright (C) Sebastian Nordgren 
// October 6th 2015
//

#pragma once

struct ImDrawData;
struct ImGuiContext;

namespace dd
{
	struct IDebugPanel;

	struct IDebugUI
	{
		virtual void RenderDebugPanels() = 0;

		virtual bool ShouldDraw() const = 0;
		virtual void SetDraw(bool draw) = 0;

		virtual bool HasMouseCapture() const = 0;
		virtual void SetMouseCapture(bool capture) = 0;

		virtual void StartFrame(float delta_t) = 0;
		virtual void EndFrame() = 0;

		virtual bool IsMidFrame() const = 0;
		virtual bool IsMidWindow() const = 0;
		virtual void EndWindow() = 0;

	private:
		friend struct IDebugPanel;

		virtual void RegisterDebugPanel(IDebugPanel& debug_panel) = 0;
	};

	struct ImGuiDebugUI : IDebugUI
	{
		ImGuiDebugUI();
		~ImGuiDebugUI();

		void StartFrame(float delta_t);
		void EndFrame();

		bool ShouldDraw() const override { return m_draw; }
		void SetDraw(bool draw) override { m_draw = draw; }

		bool HasMouseCapture() const override;
		void SetMouseCapture(bool capture) override;

		bool IsMidFrame() const override { return m_midFrame; }

		bool IsMidWindow() const override { return m_midWindow; }
		void EndWindow() override;

		void RenderDebugPanels() override;

	private:
		std::vector<IDebugPanel*> m_debugPanels;

		bool m_captureMouse { false };
		bool m_draw { false };
		bool m_midWindow { false };
		bool m_midFrame { false };
		bool m_needsSort { false };

		ImGuiContext* m_imguiContext { nullptr };

		void RegisterDebugPanel(IDebugPanel& debug_panel) override;
	};
}