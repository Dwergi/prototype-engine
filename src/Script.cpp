//
// Script.h - Wrapper around the AngelScript engine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Script.h"

namespace dd
{
	Script::Script()
	{

	}

	Script::~Script()
	{

	}

	void ScriptEngine::MessageCallback( const asSMessageInfo* msg, void* param )
	{
		const char* type = "ERR ";
		if( msg->type == asMSGTYPE_WARNING ) 
			type = "WARN";
		else if( msg->type == asMSGTYPE_INFORMATION ) 
			type = "INFO";

		printf( "%s (%d, %d): %s - %s\n", msg->section, msg->row, msg->col, type, msg->message );
	}

	ScriptEngine::ScriptEngine()
	{
		m_engine = asCreateScriptEngine( ANGELSCRIPT_VERSION );

		int r = m_engine->SetMessageCallback( asMETHOD( dd::ScriptEngine, MessageCallback ), this, asCALL_CDECL );
		ASSERT( r != 0 );
	}

	ScriptEngine::~ScriptEngine()
	{
		m_engine->ShutDownAndRelease();
		m_engine = nullptr;
	}

	void ScriptEngine::RegisterMember( const String& className, const String& sig, uint offset )
	{
		int res = m_engine->RegisterObjectProperty( className.c_str(), sig.c_str(), (int) offset );
		ASSERT( res >= 0 );
	}
}