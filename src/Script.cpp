//
// Script.h - Wrapper around the AngelScript engine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Script.h"

#include "angelscript/add_on/scriptmath/scriptmath.h"
#include "angelscript/add_on/scripthelper/scripthelper.h"

namespace dd
{
	Script::Script()
	{

	}

	Script::~Script()
	{

	}

	WriteStream* s_output = nullptr;

	void SetOutput( String* output )
	{
		if( output != nullptr )
			s_output = new WriteStream( *output );
		else
			delete s_output;
	}

	void ScriptEngine::MessageCallback( const asSMessageInfo* msg, void* param )
	{
		const char* type = "ERR ";
		if( msg->type == asMSGTYPE_WARNING ) 
			type = "WARN";
		else if( msg->type == asMSGTYPE_INFORMATION ) 
			type = "INFO";

		if( s_output == nullptr )
		{
			printf( "%s (%d, %d): %s - %s\n\n", msg->section, msg->row, msg->col, type, msg->message );
		}
		else
		{
			s_output->WriteFormat( "%s (%d, %d): %s - %s\n", msg->section, msg->row, msg->col, type, msg->message );
		}
	}

	ScriptEngine::ScriptEngine()
	{
		m_engine = asCreateScriptEngine( ANGELSCRIPT_VERSION );

		int r = m_engine->SetMessageCallback( asMETHOD( dd::ScriptEngine, MessageCallback ), this, asCALL_THISCALL );
		ASSERT( r >= 0 );

		RegisterScriptMath( m_engine );
	}

	ScriptEngine::~ScriptEngine()
	{
		m_engine->ShutDownAndRelease();
		m_engine = nullptr;
	}

	void ScriptEngine::RegisterMember( const String& className, const Member& member )
	{
		String64 objType( GetWithoutNamespace( className ) );

		String128 signature;
		signature += GetWithoutNamespace( member.Type()->Name() );
		signature += " ";
		signature += member.Name();

		int res = m_engine->RegisterObjectProperty( objType.c_str(), signature.c_str(), (int) member.Offset() );
		ASSERT( res >= 0 );
	}

	bool ScriptEngine::Evaluate( const String& script, String& output )
	{
		SetOutput( &output );

		int r = ExecuteString( m_engine, script.c_str(), m_engine->GetModule( "console", asGM_CREATE_IF_NOT_EXISTS ), nullptr );

		SetOutput( nullptr );

		if( r < 0 )
		{
			return false;
		}
		else if( r == asEXECUTION_EXCEPTION )
		{
			return false;
		}

		return true;
	}

	String256 ScriptEngine::GetFunctionSignatureString( const String& name, const Function& fn )
	{
		const FunctionSignature* sig = fn.Signature();
		String256 signature;

		if( sig->GetRet() != nullptr )
			signature += GetWithoutNamespace( sig->GetRet()->Name() );
		else
			signature += "void";

		signature += " ";
		signature += name;

		signature += "(";

		uint argCount = sig->ArgCount();
		for( uint i = 0; i < argCount; ++i )
		{
			signature += GetWithoutNamespace( sig->GetArg( i )->Name() );

			if( i < (argCount - 1) )
				signature += ",";
		}

		signature += ")";

		return signature;
	}

	String64 ScriptEngine::GetWithoutNamespace( const String& typeName )
	{
		String64 result( typeName );

		uint afterColon = 0;
		while( true )
		{
			uint res = typeName.Find( "::", afterColon );
			if( res == -1 )
				break;

			afterColon = res + 2;
		}

		if( afterColon > 0 )
		{
			result = typeName.c_str() + afterColon;
			return result;
		}

		uint ptr = 0;
		while( true )
		{
			ptr = result.Find( "*", ptr );
			if( ptr == -1 )
				break;

			result[ptr] = '@';
			++ptr;
		}

		return result;
	}

	void ScriptEngine::RegisterGlobalVariable( const String& name, const Variable& var )
	{
		String128 signature;
		signature += GetWithoutNamespace( var.Type()->Name() );
		signature += " ";
		signature += name;

		int res = m_engine->RegisterGlobalProperty( signature.c_str(), var.Data() );
		ASSERT( res >= 0 );
	}
}