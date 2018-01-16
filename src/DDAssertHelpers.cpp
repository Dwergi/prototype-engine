//
// DDAssertHelpers.cpp - Wrappers to display an ImGui assert dialog.
// Copyright (C) Sebastian Nordgren 
// January 14th 2018
//

#include "PrecompiledHeader.h"
#include "DDAssertHelpers.h"

#include "imgui/imgui.h"

namespace dd
{
	String256 FormatAssert( int level, const char* file, int line, const char* function, const char* expression )
	{
		String256 out;
		switch( level ) 
		{
		case (int) AssertLevel::Debug:
			out += "DEBUG";
			break;

		case (int) AssertLevel::Warning:
			out += "WARNING"; 
			break;

		case (int) AssertLevel::Error:
			out += "ERROR";
			break;

		case (int) AssertLevel::Fatal:
			out += "FATAL";
			break;
		}

		char buffer[1024];
		snprintf( buffer, 1024, ": \"%s\" in %s() (%s:%d)", expression, function, file, line );

		out += buffer;

		return out;
	}

	void DrawAssertDialog( glm::ivec2 window_size, Assert& assert )
	{
		ImGui::SetNextWindowPos( ImVec2( window_size.x / 2.0f - window_size.x / 6.5f, window_size.y / 2.0f - window_size.y / 6.5f ), ImGuiSetCond_FirstUseEver );

		if( ImGui::Begin( "Assert", &assert.Open ) )
		{
			ImGui::Text( assert.Info.c_str() );
			ImGui::Text( assert.Message.c_str() );
			ImGui::Spacing();

			if( ImGui::Button( "Break" ) )
			{
				assert.Action = AssertAction::Break;
				assert.Open = false;
			}

			ImGui::SameLine();

			if( ImGui::Button( "Ignore" ) )
			{
				assert.Action = AssertAction::Ignore;
				assert.Open = false;
			}

			ImGui::SameLine();

			if( ImGui::Button( "Ignore This" ) )
			{
				assert.Action = AssertAction::IgnoreLine;
				assert.Open = false;
			}

			ImGui::SameLine();

			if( ImGui::Button( "Abort" ) )
			{
				assert.Action = AssertAction::Abort;
				assert.Open = false;
			}

			ImGui::End();
		}
		else
		{
			assert.Action = AssertAction::Ignore;
		}
	}
}