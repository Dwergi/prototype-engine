
#include "PrecompiledHeader.h"
#include "IDebugPanel.h"

#include "imgui/imgui.h"

namespace dd
{
	void IDebugPanel::DrawDebugPanel()
	{
		ImGui::SetNextWindowPos( ImVec2( 30, 30 ), ImGuiSetCond_FirstUseEver );

		if( !ImGui::Begin( GetDebugTitle(), &m_debugOpen, ImVec2( 0, 0 ), 0.4f, ImGuiWindowFlags_NoSavedSettings ) )
		{
			ImGui::End();
			return;
		}

		DrawDebugInternal();

		ImGui::End();
	}

	void IDebugPanel::AddToMenu()
	{
		ImGui::MenuItem( GetDebugTitle(), nullptr, &m_debugOpen );
	}
}