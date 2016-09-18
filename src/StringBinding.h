//
// StringBinding.h - Wrapper to register our implementation of strings with AngelScript.
// Copyright (C) Sebastian Nordgren 
// June 2nd 2016
//

#pragma once

namespace dd
{
	class AngelScriptEngine;

	void RegisterString( AngelScriptEngine& engine );
}