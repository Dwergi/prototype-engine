//
// IDebugPanel.cpp
// Copyright (C) Sebastian Nordgren 
// May 2nd 2017
//

#include "PCH.h"
#include "IDebugPanel.h"

#include "DebugUI.h"

namespace dd
{
	static dd::Service<dd::DebugUI> s_debugUI;

	IDebugPanel::IDebugPanel()
	{
		s_debugUI->RegisterDebugPanel(*this);
	}

	void IDebugPanel::DrawDebugPanel()
	{
		ImGui::SetNextWindowPos(ImVec2(30, 30), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowBgAlpha(0.4f);

		if (ImGui::Begin(GetDebugTitle(), &m_debugOpen, ImGuiWindowFlags_NoSavedSettings))
		{
			DrawDebugInternal();
		}

		ImGui::End();
	}
}

namespace ImGui
{
	void Value(const char* prefix, const glm::ivec2& value)
	{
		ImGui::Text("%s: %.3f, %.3f", prefix, value.x, value.y);
	}

	void Value(const char* prefix, const glm::vec2& value, const char* float_format)
	{
		if (float_format != nullptr)
		{
			char fmt[64];
			snprintf(fmt, 64, "%%s: %s, %s", float_format, float_format);
			ImGui::Text(fmt, prefix, value.x, value.y);
		}
		else
		{
			ImGui::Text("%s: %.3f, %.3f", prefix, value.x, value.y);
		}
	}

	void Value(const char* prefix, const glm::vec3& value, const char* float_format)
	{
		if (float_format != nullptr)
		{
			char fmt[64];
			snprintf(fmt, 64, "%%s: %s, %s, %s", float_format, float_format, float_format);
			ImGui::Text(fmt, prefix, value.x, value.y, value.z);
		}
		else
		{
			ImGui::Text("%s: %.3f, %.3f, %.3f", prefix, value.x, value.y, value.z);
		}
	}

	void Value(const char* prefix, const glm::vec4& value, const char* float_format)
	{
		if (float_format != nullptr)
		{
			char fmt[64];
			snprintf(fmt, 64, "%%s: %s, %s, %s, %s", float_format, float_format, float_format, float_format);
			ImGui::Text(fmt, prefix, value.x, value.y, value.z, value.w);
		}
		else
		{
			ImGui::Text("%s: %.3f, %.3f, %.3f, %.3f", prefix, value.x, value.y, value.z, value.w);
		}
	}

	void Value(const char* prefix, const uint64& value)
	{
		ImGui::Text("%s: %llu", prefix, value);
	}
}