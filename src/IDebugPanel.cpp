
#include "PCH.h"
#include "IDebugPanel.h"



namespace dd
{
	void IDebugPanel::DrawDebugPanel( ddc::World& world )
	{
		ImGui::SetNextWindowPos( ImVec2( 30, 30 ), ImGuiCond_FirstUseEver );

		if( ImGui::Begin( GetDebugTitle(), &m_debugOpen, ImVec2( 0, 0 ), 0.4f, ImGuiWindowFlags_NoSavedSettings ) )
		{
			DrawDebugInternal( world );
		}

		ImGui::End();
	}
}