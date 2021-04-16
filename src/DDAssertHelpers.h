//
// DDAssertHelpers.h - Wrappers to display an ImGui assert dialog.
// Copyright (C) Sebastian Nordgren 
// January 14th 2018
//

#pragma once

namespace dd::Assert
{
	void Initialize();
	void Shutdown();
	void Check();
}