//
// DDAssertHelpers.h - Wrappers to display an ImGui assert dialog.
// Copyright (C) Sebastian Nordgren 
// January 14th 2018
//

#pragma once

namespace dd
{
	struct Assert
	{
		String256 Info;
		String256 Message;
		bool Open { false };
		AssertAction Action;
	};

	String256 FormatAssert( int level, const char* file, int line, const char* function, const char* expression );
	void DrawAssertDialog( glm::ivec2 window_size, Assert& assert );
}