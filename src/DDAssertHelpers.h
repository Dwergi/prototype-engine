//
// DDAssertHelpers.h - Wrappers to display an ImGui assert dialog.
// Copyright (C) Sebastian Nordgren 
// January 14th 2018
//

#pragma once

namespace dd
{
	void InitializeAssert( std::thread::id main_thread );
	void CheckAssert();
}