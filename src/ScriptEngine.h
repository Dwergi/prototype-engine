//
// ScriptEngine.h - Wrapper around the AngelScript engine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "RefCounter.h"

#include "angelscript/include/angelscript.h"

namespace dd
{
	class Script;

	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();

		Script& LoadScript( const char* script );
		void UnloadScript( Script& script );

		template <typename FnType>
		void RegisterMethod( const char* name, const Function& method, FnType ptr );

		// 
		// Register a script object that is passed by reference exclusively.
		// 
		template <typename ObjType>
		void RegisterObject( const String& className );

		//
		// Register a script object that is passed by value.
		//
		template <typename ObjType>
		void RegisterStruct( const String& className );

		void RegisterMember( const String& className, const Member& member );

		template <typename FnType>
		void RegisterGlobalFunction( const String& name, const Function& function, FnType ptr, const char* explicit_sig = nullptr );

		void RegisterGlobalVariable( const String& name, const Variable& var );

		bool IsObjectRegistered( const String& className );

		bool Evaluate( const String& script, String& output );

		asIScriptEngine* GetInternalEngine() const { return m_engine; }

	private:

		Vector<Script> m_vecScripts;
		asIScriptEngine* m_engine;

		void MessageCallback( const asSMessageInfo* msg, void* param );
		static String64 GetWithoutNamespace( const String& className );

		static String256 GetFunctionSignatureString( const String& name, const Function& fn );
	};
	//---------------------------------------------------------------------------

	#include "ScriptEngine.inl"
}