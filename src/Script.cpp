//
// Script.h - Wrapper around the AngelScript engine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Script.h"

#include "angelscript/include/angelscript.h"

dd::Script::Script()
{

}

dd::Script::~Script()
{

}

void dd::ScriptEngine::MessageCallback( const asSMessageInfo* msg, void* param )
{
	const char* type = "ERR ";
	if( msg->type == asMSGTYPE_WARNING ) 
		type = "WARN";
	else if( msg->type == asMSGTYPE_INFORMATION ) 
		type = "INFO";

	printf( "%s (%d, %d): %s - %s\n", msg->section, msg->row, msg->col, type, msg->message );
}

dd::ScriptEngine::ScriptEngine()
{
	m_engine = asCreateScriptEngine( ANGELSCRIPT_VERSION );

	int r = m_engine->SetMessageCallback( asMETHOD( dd::ScriptEngine, MessageCallback ), this, asCALL_CDECL );
	ASSERT( r != 0 );
}
/*

void dd::ScriptEngine::RegisterMethod( dd::MethodBase& method )
{
	int r = method.RegisterMethod( m_engine );

	ASSERT( r >= 0 );
}*/