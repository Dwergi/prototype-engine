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

#include "Stream.h"

namespace dd
{

	AngelScriptFunction::~AngelScriptFunction()
	{
		Invalidate();
	}

	AngelScriptFunction::AngelScriptFunction( AngelScriptFunction&& other )
	{
		m_context = nullptr;

		std::swap( m_context, other.m_context );
		std::swap( m_function, other.m_function );
	}

	AngelScriptFunction& AngelScriptFunction::operator=( AngelScriptFunction&& other )
	{
		std::swap( m_context, other.m_context );
		std::swap( m_function, other.m_function );
		std::swap( m_engine, other.m_engine );

		return *this;
	}

	AngelScriptFunction::AngelScriptFunction( AngelScriptEngine* engine, asIScriptFunction* fn )
	{
		m_function = fn;
		m_engine = engine;
		m_context = nullptr;
	}

	bool AngelScriptFunction::Valid() const
	{
		return m_engine != nullptr && m_function != nullptr;
	}

	void AngelScriptFunction::Invalidate()
	{
		ReleaseContext();

		m_engine = nullptr;
		m_function = nullptr;
	}

	void AngelScriptFunction::ReleaseContext()
	{
		if( m_context != nullptr )
		{
			m_context->Release();
			m_context = nullptr;
		}
	}

	void AngelScriptEngine::SetOutput( String* output )
	{
		if( output != nullptr )
			m_output = new WriteStream( *output );
		else
			delete m_output;
	}

	void AngelScriptEngine::MessageCallback( const asSMessageInfo* msg, void* param )
	{
		const char* type = "ERR ";
		if( msg->type == asMSGTYPE_WARNING ) 
			type = "WARN";
		else if( msg->type == asMSGTYPE_INFORMATION ) 
			type = "INFO";

		if( m_output == nullptr )
		{
			printf( "%s (%d, %d): %s - %s\n\n", msg->section, msg->row, msg->col, type, msg->message );
		}
		else
		{
			m_output->WriteFormat( "%s (%d, %d): %s - %s\n", msg->section, msg->row, msg->col, type, msg->message );
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

	AngelScriptFunction* AngelScriptEngine::GetFunction( const char* module, const char* functionSig )
	{
		asIScriptModule* mod = m_engine->GetModule( module );
		if( module != nullptr )
		{
			asIScriptFunction* func = mod->GetFunctionByDecl( functionSig );
			if( func != nullptr )
			{
				return new AngelScriptFunction( this, func );
			}
		}

		return nullptr;
	}
}