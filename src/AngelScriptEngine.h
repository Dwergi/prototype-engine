//
// AngelScriptEngine.h - Wrapper around the AngelScript engine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include <angelscript.h>

namespace dd
{
	class Script;

	class AngelScriptEngine
	{
	public:
		AngelScriptEngine();
		~AngelScriptEngine();

		// disable copying
		AngelScriptEngine( const AngelScriptEngine& ) = delete;
		AngelScriptEngine& operator=( const AngelScriptEngine& ) = delete;

		template <typename FnType>
		void RegisterMethod( const char* name, const Function& method, FnType ptr );

		// 
		// Register a script object that is passed by reference exclusively.
		// 
		template <typename T>
		void RegisterObject( const char* className );

		//
		// Register a script object that is passed by value.
		//
		template <typename T>
		void RegisterStruct( const char* className );

		void RegisterMember( const char* className, const Member& member );

		template <typename FnType>
		void RegisterGlobalFunction( const char* name, const Function& function, FnType ptr );

		template <typename T, T& Variable>
		void RegisterGlobalVariable( const char* name );

		bool Evaluate( const String& script, String& output );

		asIScriptEngine* GetInternalEngine() const { return m_engine; }

		BASIC_TYPE( AngelScriptEngine )

	private:

		Vector<Script> m_vecScripts;
		asIScriptEngine* m_engine;

		void MessageCallback( const asSMessageInfo* msg, void* param );
		static String64 ReplacePointer( const char* typeName );

		static String256 GetFunctionSignatureString( const char* name, const Function& fn );
	};
	//---------------------------------------------------------------------------

	#include "AngelScriptEngine.inl"
}