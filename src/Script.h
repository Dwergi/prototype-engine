//
// Script.h - Wrapper around the AngelScript engine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "angelscript/include/angelscript.h"

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
		static ScriptEngine* GetInstance()
		{
			static ScriptEngine s_instance;
			return &s_instance;
		}

		~ScriptEngine();

		Script& LoadScript( const char* script );
		void UnloadScript( Script& );

		template <typename FnType>
		void RegisterMethod( const String& className, const String& signature, FnType method );

	private:

		Vector<Script> m_vecScripts;
		asIScriptEngine* m_engine;

		ScriptEngine();

		void MessageCallback( const asSMessageInfo* msg, void* param );
	};

	template <typename FnType>
	void ScriptEngine::RegisterMethod( const String& className, const String& signature, FnType method )
	{
		class DummyClass {};
		const int METHOD_SIZE = sizeof(void (DummyClass::*)());

		//int r = m_engine->RegisterObjectMethod( className.c_str(), signature.c_str(), asSMethodPtr<METHOD_SIZE>::Convert( method ), asCALL_THISCALL );
		//ASSERT( r >= 0, "Failed to register method \'%s\' for class \'%s\'!", signature.c_str(), className.c_str() );
	}
}