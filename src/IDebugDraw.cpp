#include "PrecompiledHeader.h"
#include "IDebugDraw.h"

#include "imgui/imgui.h"

namespace dd
{
	void IDebugDraw::DrawDebug()
	{
		ImGui::SetNextWindowPos( ImVec2( 30, 30 ), ImGuiSetCond_FirstUseEver );

		if( !ImGui::Begin( GetDebugTitle(), &IsDebugOpen(), ImVec2( 0, 0 ), 0.4f, ImGuiWindowFlags_NoSavedSettings ) )
		{
			ImGui::End();
			return;
		}

		DrawDebugInternal();

		ImGui::End();
	}
}