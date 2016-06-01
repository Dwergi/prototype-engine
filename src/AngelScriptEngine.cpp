//
// AngelScriptEngine.cpp - Wrapper around the AngelScript engine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "AngelScriptEngine.h"

#include "angelscript/add_on/scriptmath/scriptmath.h"
#include "angelscript/add_on/scripthelper/scripthelper.h"
#include "angelscript/add_on/scriptbuilder/scriptbuilder.h"

#include <direct.h>

namespace dd
{
	WriteStream* s_output = nullptr;

	void SetOutput( String* output )
	{
		if( output != nullptr )
			s_output = new WriteStream( *output );
		else
			delete s_output;
	}

	void AngelScriptEngine::MessageCallback( const asSMessageInfo* msg, void* param )
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

	AngelScriptEngine::AngelScriptEngine()
	{
		m_engine = asCreateScriptEngine( ANGELSCRIPT_VERSION );

		int r = m_engine->SetMessageCallback( asMETHOD( dd::AngelScriptEngine, MessageCallback ), this, asCALL_THISCALL );
		DD_ASSERT( r >= 0 );

		RegisterScriptMath( m_engine );
	}

	AngelScriptEngine::~AngelScriptEngine()
	{
		m_engine->ShutDownAndRelease();
		m_engine = nullptr;
	}

	bool AngelScriptEngine::Evaluate( const String& script, String& output )
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

	String256 AngelScriptEngine::GetFunctionSignatureString( const char* name, const Function& fn )
	{
		const FunctionSignature* sig = fn.Signature();
		String256 signature;

		if( sig->GetRet() != nullptr )
			signature += ReplacePointer( sig->GetRet()->Name().c_str() );
		else
			signature += "void";

		signature += " ";
		signature += name;

		signature += "(";

		uint argCount = sig->ArgCount();
		for( uint i = 0; i < argCount; ++i )
		{
			signature += ReplacePointer( sig->GetArg( i )->Name().c_str() );

			if( i < (argCount - 1) )
				signature += ",";
		}

		signature += ")";

		return signature;
	}

	String64 AngelScriptEngine::ReplacePointer( const char* type )
	{
		String64 result( type );
		result.ReplaceAll( '*', '@' );

		return result;
	}

	String256 LoadScript( const char* module )
	{
		const char* path = "\\data\\as\\";

		char current[256];
		_getcwd( current, 256 );

		String256 filename( current );
		filename += path;
		filename += module;
		filename += ".as";

		std::FILE* file;
		fopen_s( &file, filename.c_str(), "rb" );

		if( file == nullptr )
			return String256();

		char buffer[2048];
		String256 source;

		while( std::fgets( buffer, 2048, file ) != nullptr )
		{
			source += buffer;
		}

		fclose( file );

		return source;
	}

	bool AngelScriptEngine::LoadFile( const char* module, String& output )
	{
		dd::String256 strSource( LoadScript( module ) );

		SetOutput( &output );

		CScriptBuilder builder;
		int r = builder.StartNewModule( m_engine, module );

		if( r >= 0 )
		{
			r = builder.AddSectionFromMemory( module, strSource.c_str(), strSource.Length() );
			if( r >= 0 )
			{
				r = builder.BuildModule();
			}
		}

		SetOutput( nullptr );

		return r >= 0;
	}

	bool AngelScriptEngine::RunFunction( const char* module, const String& functionSig, String& output )
	{
		SetOutput( &output );

		bool result = false;

		asIScriptModule* mod = m_engine->GetModule( module );
		if( module != nullptr )
		{
			asIScriptFunction* func = mod->GetFunctionByDecl( functionSig.c_str() );
			if( func != nullptr )
			{
				asIScriptContext* context = m_engine->CreateContext();
				if( context != nullptr )
				{
					context->Prepare( func );

					int r = context->Execute();
					if( r == asEXECUTION_FINISHED )
					{
						result = true;
					}

					context->Release();
				}
			}
		}

		SetOutput( nullptr );
		return result;
	}
}