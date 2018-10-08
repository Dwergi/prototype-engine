//
// AngelScriptEngine.cpp - Wrapper around the AngelScript engine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
#include "AngelScriptEngine.h"

#include "angelscript/add_on/scriptmath/scriptmath.h"
#include "angelscript/add_on/scripthelper/scripthelper.h"
#include "angelscript/add_on/scriptbuilder/scriptbuilder.h"

#include <direct.h>

#include <fmt/format.h>

#include "File.h"

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

	void AngelScriptEngine::MessageCallback( const asSMessageInfo* msg )
	{
		const char* type = "ERR";
		if( msg->type == asMSGTYPE_WARNING ) 
			type = "WARN";
		else if( msg->type == asMSGTYPE_INFORMATION ) 
			type = "INFO";

		if( m_output == nullptr )
		{
			fmt::print( "{} ({}, {}): {} - {}\n", msg->section, msg->row, msg->col, type, msg->message );
		}
		else
		{
			*m_output = fmt::format( "{} ({}, {}): {} - {}\n", msg->section, msg->row, msg->col, type, msg->message );
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

	bool AngelScriptEngine::Evaluate( std::string script, std::string& output )
	{
		m_output = &output;

		int r = ExecuteString( m_engine, script.c_str(), m_engine->GetModule( "console", asGM_CREATE_IF_NOT_EXISTS ), nullptr );

		m_output = nullptr;

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

		String64 ret_type( "void" );
		if( sig->GetRet() != nullptr )
		{
			ret_type = ReplacePointer( sig->GetRet()->Name() );
		}

		std::string sig_str = fmt::format( "{} {}(", ret_type.c_str(), name );

		uint argCount = sig->ArgCount();
		for( uint i = 0; i < argCount; ++i )
		{
			sig_str.append( ReplacePointer( sig->GetArg( i )->Name() ).c_str() );

			if( i < (argCount - 1) )
				sig_str.append( "," );
		}

		sig_str.append( ")" );

		return String256( sig_str.c_str() );
	}

	String64 AngelScriptEngine::ReplacePointer( const String& typeName )
	{
		String64 result( typeName );
		result.ReplaceAll( '*', '@' );

		return result;
	}

	String256 AngelScriptEngine::LoadSource( const char* mod ) const
	{
		DD_ASSERT( mod != nullptr );

		std::string filename = fmt::format( "as/{}.as", mod );

		File file( filename );

		std::string source;
		if( !file.Read( source ) )
		{
			std::string error = fmt::format( "Failed to load file containing module from path: {}", filename );

			m_engine->WriteMessage( mod, 0, 0, asMSGTYPE_ERROR, error.c_str() );
			return String256();
		}

		return String256( source.c_str() );
	}

	bool AngelScriptEngine::LoadFile( const char* mod, std::string& output )
	{
		DD_ASSERT( mod != nullptr );

		m_output = &output;

		dd::String256 strSource( LoadSource( mod ) );

		CScriptBuilder builder;
		int r = builder.StartNewModule( m_engine, mod );

		if( r >= 0 )
		{
			r = builder.AddSectionFromMemory( mod, strSource.c_str(), strSource.Length() );
			if( r >= 0 )
			{
				r = builder.BuildModule();
			}
		}

		m_output = nullptr;

		return r >= 0;
	}

	bool AngelScriptEngine::IsModuleLoaded( const char* mod ) const
	{
		asIScriptModule* asMod = m_engine->GetModule( mod );
		return asMod != nullptr;
	}

	AngelScriptObject* AngelScriptEngine::GetScriptObject( const char* mod, const char* className )
	{
		asIScriptModule* asMod = m_engine->GetModule( mod );
		if( asMod != nullptr )
		{
			asITypeInfo* typeInfo = asMod->GetTypeInfoByDecl( className );
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

	AngelScriptFunction* AngelScriptEngine::GetFunction( const char* mod, const char* functionSig )
	{
		asIScriptModule* asMod = m_engine->GetModule( mod );
		if( asMod != nullptr )
		{
			asIScriptFunction* func = asMod->GetFunctionByDecl( functionSig );
			if( func != nullptr )
			{
				return new AngelScriptFunction( this, func, nullptr );
			}
		}

		return nullptr;
	}
}