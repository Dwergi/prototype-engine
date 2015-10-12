//
// Script.h - Wrapper around the AngelScript engine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "RefCounter.h"

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
	//---------------------------------------------------------------------------

	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();

		Script& LoadScript( const char* script );
		void UnloadScript( Script& );

		template <typename FnType>
		void RegisterMethod( const String& className, const String& signature, FnType method );

		template <typename ObjType>
		void RegisterObject( const String& className );

		void RegisterMember( const String& className, const String& sig, uint offset );

		bool IsObjectRegistered( const String& className );

		bool Evaluate( const String& script, String& output );

		void RegisterConsoleHelpers();

		asIScriptEngine* GetInternalEngine() const { return m_engine; }

	private:

		Vector<Script> m_vecScripts;
		asIScriptEngine* m_engine;

		void MessageCallback( const asSMessageInfo* msg, void* param );
		static String64 GetWithoutNamespace( const String& className );
	};
	//---------------------------------------------------------------------------

	template <typename T>
	T* Factory()
	{
		T* ptr = new T();
		ptr->m_refCount.Init( ptr );
		return ptr;
	}

	template <typename FnType>
	void ScriptEngine::RegisterMethod( const String& className, const String& signature, FnType method )
	{
		class DummyClass {};
		const int METHOD_SIZE = sizeof(void (DummyClass::*)());

		int res = m_engine->RegisterObjectMethod( className.c_str(), signature.c_str(), asSMethodPtr<METHOD_SIZE>::Convert( method ), asCALL_THISCALL );
		ASSERT( res >= 0, "Failed to register method \'%s\' for class \'%s\'!", signature.c_str(), className.c_str() );
	}

	template <typename ObjType>
	void ScriptEngine::RegisterObject( const String& className )
	{
		int res = m_engine->RegisterObjectType( className.c_str(), 0, asOBJ_REF );
		ASSERT( res >= 0 );

		String32 strSig = className;
		strSig += "@ Factory()";

		res = m_engine->RegisterObjectBehaviour( className.c_str(), asBEHAVE_FACTORY, strSig.c_str(), asFUNCTION( Factory<ObjType> ), asCALL_CDECL );
		ASSERT( res >= 0 );
		
		res = m_engine->RegisterObjectBehaviour( className.c_str(), asBEHAVE_ADDREF, "void Increment()", asMETHOD( RefCounter<ObjType>, Increment ), asCALL_THISCALL );
		ASSERT( res >= 0 );

		res = m_engine->RegisterObjectBehaviour( className.c_str(), asBEHAVE_RELEASE, "void Decrement()", asMETHOD( RefCounter<ObjType>, Decrement ), asCALL_THISCALL );
		ASSERT( res >= 0 );
	}
}