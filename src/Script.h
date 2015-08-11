//
// Script.h - Wrapper around the AngelScript engine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Vector.h"

class asIScriptEngine;
struct asSMessageInfo;

namespace dd
{
	class ScriptEngine;

	class Script
	{
	public:

		Script();
		Script( const ScriptEngine& engine, const char* script );
		~Script();
	};

	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();

		Script& LoadScript( const char* script );
		void UnloadScript( Script& );

		void RegisterMethod( dd::MethodBase& method );

	private:

		Vector<Script> m_vecScripts;
		asIScriptEngine* m_engine;

		void MessageCallback( const asSMessageInfo* msg, void* param );
	};
}