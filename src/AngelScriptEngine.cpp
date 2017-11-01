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

#include "File.h"
#include "Stream.h"

namespace dd
{

	AngelScriptFunction::~AngelScriptFunction()
	{
		ReleaseContext();

		m_engine = nullptr;
		m_function = nullptr;
	}

	AngelScriptFunction::AngelScriptFunction( AngelScriptFunction&& other )
	{
		ReleaseContext();

		std::swap( m_context, other.m_context );
		std::swap( m_function, other.m_function );
		std::swap( m_engine, other.m_engine );
		std::swap( m_object, other.m_object );
	}

	AngelScriptFunction& AngelScriptFunction::operator=( AngelScriptFunction&& other )
	{
		ReleaseContext();

		std::swap( m_context, other.m_context );
		std::swap( m_function, other.m_function );
		std::swap( m_engine, other.m_engine );
		std::swap( m_object, other.m_object );

		return *this;
	}

	AngelScriptFunction::AngelScriptFunction( AngelScriptEngine* engine, asIScriptFunction* fn, asIScriptObject* obj )
	{
		m_function = fn;
		m_engine = engine;
		m_object = obj;
	}

	bool AngelScriptFunction::IsValid() const
	{
		return m_engine != nullptr && m_function != nullptr;
	}

	void AngelScriptFunction::ReleaseContext()
	{
		if( m_context != nullptr )
		{
			m_context->Release();
			m_context = nullptr;
		}
	}

	AngelScriptObject::AngelScriptObject( AngelScriptEngine* engine, asITypeInfo* typeInfo, asIScriptObject* obj )
	{
		m_engine = engine;
		m_typeInfo = typeInfo;
		m_object = obj;
	}

	AngelScriptObject::~AngelScriptObject()
	{
		ReleaseObject();
		m_engine = nullptr;
	}

	AngelScriptObject::AngelScriptObject( AngelScriptObject&& other )
	{
		std::swap( m_engine, other.m_engine );
		std::swap( m_object, other.m_object );
		std::swap( m_typeInfo, other.m_typeInfo );
	}

	AngelScriptObject& AngelScriptObject::operator=( AngelScriptObject&& other )
	{
		ReleaseObject();
		m_engine = nullptr;

		std::swap( m_engine, other.m_engine );
		std::swap( m_object, other.m_object );
		std::swap( m_typeInfo, other.m_typeInfo );

		return *this;
	}

	bool AngelScriptObject::IsValid() const
	{
		return m_engine != nullptr && m_typeInfo != nullptr && m_object != nullptr;
	}

	void AngelScriptObject::ReleaseObject()
	{
		if( m_object != nullptr )
		{
			m_object->Release();
			m_object = nullptr;
		}
	}

	AngelScriptFunction* AngelScriptObject::GetMethod( const char* signature )
	{
		asIScriptFunction* fn = m_typeInfo->GetMethodByDecl( signature );
		if( fn != nullptr )
		{
			return new AngelScriptFunction( m_engine, fn, m_object );
		}

		return nullptr;
	}

	void AngelScriptEngine::SetOutput( String* output )
	{
		if( output != nullptr )
			m_output = new WriteStream( *output );
		else
			delete m_output;
	}

	void AngelScriptEngine::MessageCallback( const asSMessageInfo* msg )
	{
		const char* type = "ERR";
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
		m_engine = asCreateScriptEngine();

		int r = m_engine->SetMessageCallback( asMETHOD( dd::AngelScriptEngine, MessageCallback ), this, asCALL_THISCALL );
		DD_ASSERT( r >= 0 );

		RegisterScriptMath( m_engine );
	}

	AngelScriptEngine::~AngelScriptEngine()
	{
		m_engine->ClearMessageCallback();
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

	String256 AngelScriptEngine::LoadSource( const char* module ) const
	{
		DD_ASSERT( module != nullptr );

		String256 filename;
		filename += "as/";
		filename += module;
		filename += ".as";

		std::unique_ptr<File> file = File::OpenDataFile( filename, File::Mode::Read );
		if( file == nullptr )
		{
			String256 error;
			error += "Failed to load file containing module from path: ";
			error += filename;

			m_engine->WriteMessage( module, 0, 0, asMSGTYPE_ERROR, error.c_str() );
			return String256();
		}

		char buffer[2048];
		String256 source;

		int read = 0;
		while( (read = file->Read( (byte*) buffer, 2048 )) == 2048 )
		{
			source.Append( buffer, 2048 );
		}

		// copy the last chunk
		source.Append( buffer, (uint) read );

		return source;
	}

	bool AngelScriptEngine::LoadFile( const char* module, String& output )
	{
		DD_ASSERT( module != nullptr );

		SetOutput( &output );

		dd::String256 strSource( LoadSource( module ) );

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

	bool AngelScriptEngine::IsModuleLoaded( const char* module ) const
	{
		asIScriptModule* mod = m_engine->GetModule( module );
		return mod != nullptr;
	}

	AngelScriptObject* AngelScriptEngine::GetScriptObject( const char* module, const char* className )
	{
		asIScriptModule* mod = m_engine->GetModule( module );
		if( module != nullptr )
		{
			asITypeInfo* typeInfo = mod->GetTypeInfoByDecl( className );
			if( typeInfo != nullptr )
			{
				String256 factoryName;
				factoryName += className;
				factoryName += " @";
				factoryName += className;
				factoryName += "()";

				asIScriptFunction* factory = typeInfo->GetFactoryByDecl( factoryName.c_str() );
				if( factory != nullptr )
				{
					asIScriptContext* context = m_engine->CreateContext();
					if( context != nullptr )
					{
						context->Prepare( factory );
						context->Execute();
						
						asIScriptObject* obj = *(asIScriptObject**) context->GetAddressOfReturnValue();
						if( obj != nullptr )
						{
							obj->AddRef();
							return new AngelScriptObject( this, typeInfo, obj );
						}
					}
				}
			}
		}

		return nullptr;
	}

	AngelScriptFunction* AngelScriptEngine::GetFunction( const char* module, const char* functionSig )
	{
		asIScriptModule* mod = m_engine->GetModule( module );
		if( module != nullptr )
		{
			asIScriptFunction* func = mod->GetFunctionByDecl( functionSig );
			if( func != nullptr )
			{
				return new AngelScriptFunction( this, func, nullptr );
			}
		}

		return nullptr;
	}
}