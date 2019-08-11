//
// IDebugDraw.h - Interface for classes that do debug drawing.
// Copyright (C) Sebastian Nordgren 
// May 2nd 2017
//

#pragma once

#include "Typedefs.h"

#include "imgui/imgui.h"

namespace ddc
{
	struct EntitySpace;
}

namespace dd
{
	struct IDebugPanel
	{
		IDebugPanel() :
			m_debugOpen( false )
		{
		}

		void DrawDebugPanel();

		//
		// Is this debug menu open?
		//
		bool IsDebugPanelOpen() const { return m_debugOpen; }

		//
		// Change the open status of this menu.
		//
		void SetDebugPanelOpen( bool value ) { m_debugOpen = value; }

		//
		// Get the display name of the debug panel.
		// 
		virtual const char* GetDebugTitle() const = 0;


	private:

		bool m_debugOpen;

		//
		// Override this to draw your debug UI. You may assume that IsDebugOpen() is true, and that a window already exists. Do NOT call ImGui::End().
		//
		virtual void DrawDebugInternal() = 0;
	};
}

namespace ImGui
{
	void Value(const char* prefix, const uint64& value);
	void Value(const char* prefix, const glm::ivec2& value);
	void Value(const char* prefix, const glm::vec2& value, const char* float_format = nullptr);
	void Value(const char* prefix, const glm::vec3& value, const char* float_format = nullptr);
	void Value(const char* prefix, const glm::vec4& value, const char* float_format = nullptr);
}