//
// Script.h - Wrapper around the AngelScript engine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Script.h"

#include "angelscript/add_on/scriptmath/scriptmath.h"
#include "angelscript/add_on/scripthelper/scripthelper.h"

namespace
{
	dd::WriteStream* s_stream = nullptr;
	void SetGrabInto( dd::String* out )
	{
		if( s_stream != nullptr )
			delete s_stream;

		s_stream = new dd::WriteStream( *out );
	}

	void grab( int v )
	{
		ASSERT( s_stream != nullptr );

		SerializePOD( dd::Serialize::Mode::JSON, *s_stream, v );
	}

	void grab( asUINT v )
	{
		ASSERT( s_stream != nullptr );

		SerializePOD( dd::Serialize::Mode::JSON, *s_stream, v );
	}

	void grab( bool v )
	{
		ASSERT( s_stream != nullptr );

		SerializePOD( dd::Serialize::Mode::JSON, *s_stream, v );
	}

	void grab( float v )
	{
		ASSERT( s_stream != nullptr );

		SerializePOD( dd::Serialize::Mode::JSON, *s_stream, v );
	}

	void grab( double v )
	{
		ASSERT( s_stream != nullptr );

		SerializePOD( dd::Serialize::Mode::JSON, *s_stream, v );
	}

	void grab( const dd::String& v )
	{
		ASSERT( s_stream != nullptr );

		SerializeString( dd::Serialize::Mode::JSON, *s_stream, v );
	}

	void grab()
	{
		// There is no value
	}
}

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

		RegisterScriptMath( m_engine );
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

	void ScriptEngine::RegisterConsoleHelpers()
	{
		int res = 0;

		// Register special function with overloads to catch any type.
		// This is used by the exec command to output the resulting value from the statement.
		res = m_engine->RegisterGlobalFunction( "void _grab(bool)", asFUNCTIONPR( grab, (bool), void ), asCALL_CDECL ); 
		ASSERT( res >= 0 );
		res = m_engine->RegisterGlobalFunction( "void _grab(int)", asFUNCTIONPR( grab, (int), void ), asCALL_CDECL );
		ASSERT( res >= 0 );
		res = m_engine->RegisterGlobalFunction( "void _grab(uint)", asFUNCTIONPR( grab, (asUINT), void ), asCALL_CDECL );
		ASSERT( res >= 0 );
		res = m_engine->RegisterGlobalFunction( "void _grab(float)", asFUNCTIONPR( grab, (float), void ), asCALL_CDECL );
		ASSERT( res >= 0 );
		res = m_engine->RegisterGlobalFunction( "void _grab(double)", asFUNCTIONPR( grab, (double), void ), asCALL_CDECL );
		ASSERT( res >= 0 );
		res = m_engine->RegisterGlobalFunction( "void _grab()", asFUNCTIONPR( grab, (void), void ), asCALL_CDECL );
		ASSERT( res >= 0 );
		/*res = engine->RegisterGlobalFunction( "void _grab(const string &in)", asFUNCTIONPR( grab, (const string&), void ), asCALL_CDECL ); 
		ASSERT( res >= 0 );*/
	}

	bool ScriptEngine::Evaluate( const String& script, String& output )
	{
		String256 completeString( "_grab(" );
		completeString += script;
		completeString += ")";

		output.Clear();

		SetGrabInto( &output );

		int r = ExecuteString( m_engine, completeString.c_str(), m_engine->GetModule( "console", asGM_CREATE_IF_NOT_EXISTS ), nullptr );

		SetGrabInto( nullptr );

		if( r < 0 )
		{
			output = "Invalid script statement!";
			return false;
		}
		else if( r == asEXECUTION_EXCEPTION )
		{
			output = "A script exception was raised.";
			return false;
		}

		return true;
	}
}