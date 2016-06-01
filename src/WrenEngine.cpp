//
// WrenEngine.cpp - Wrapper around the Wren script engine.
// Copyright (C) Sebastian Nordgren 
// February 7th 2015
//

#include "PrecompiledHeader.h"
#include "WrenEngine.h"

#include <cstdio>
#include <direct.h>

#include "wren/src/vm/wren_vm.h"

namespace dd
{
	static Vector<WrenEngine*> m_activeEngines;

	//
	// Find the engine that owns the given VM.
	//
	WrenEngine* WrenEngine::FindEngine( WrenVM* vm )
	{
		for( WrenEngine* engine : m_activeEngines )
		{
			if( engine->m_vm == vm )
				return engine;
		}

		return nullptr;
	}

	//
	// Callback from Wren when something like print() is called that should write to output.
	//
	void WriteCallback( WrenVM* vm, const char* msg )
	{
		WrenEngine* engine = WrenEngine::FindEngine( vm );
		if( engine == nullptr )
			return;

		engine->Write( msg );
	}

	WrenForeignClassMethods BindForeignClassCallback( WrenVM* vm, const char* module, const char* name )
	{
		WrenForeignClassMethods ret { nullptr, nullptr };

		WrenEngine* engine = WrenEngine::FindEngine( vm );
		if( engine == nullptr )
			return ret;

		WrenEngine::WrenClass* classReg = engine->FindClass( module, name );
		if( classReg == nullptr )
			return ret;

		return classReg->ForeignMethods;
	}

	WrenForeignMethodFn	BindForeignMethodCallback( WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature )
	{
		WrenEngine* engine = WrenEngine::FindEngine( vm );

		if( engine == nullptr )
			return nullptr;

		WrenEngine::WrenClass* classReg = engine->FindClass( module, className );

		if( classReg == nullptr )
			return nullptr;

		WrenEngine::WrenFunction* funcReg = classReg->FindFunction( signature );

		if( funcReg == nullptr )
			return nullptr;

		return funcReg->Function;
	}

	char* LoadModuleCallback( WrenVM* vm, const char* module )
	{
		WrenEngine* engine = WrenEngine::FindEngine( vm );

		if( engine == nullptr )
			return nullptr;

		String256 source( engine->LoadModule( module ) );

		// reallocate so that Wren can take ownership of the string
		char* memory = (char*) std::realloc( NULL, source.Length() + 1 );
		std::memcpy( memory, source.c_str(), source.Length() + 1 );
		memory[source.Length()] = '\0';

		return memory;
	}

	WrenMethod::WrenMethod( WrenMethod&& other )
	{
		std::swap( m_vm, other.m_vm );
		std::swap( m_arity, other.m_arity );
		std::swap( m_method, other.m_method );
		std::swap( m_variable, other.m_variable );
	}

	WrenMethod& WrenMethod::operator=( WrenMethod&& other )
	{
		std::swap( m_vm, other.m_vm );
		std::swap( m_arity, other.m_arity );
		std::swap( m_method, other.m_method );
		std::swap( m_variable, other.m_variable );

		return *this;
	}

	WrenMethod::WrenMethod( WrenVM* vm, WrenValue* variable, WrenValue* method, uint arity )
	{
		m_vm = vm;
		m_variable = variable;
		m_method = method;
		m_arity = arity;
	}

	WrenMethod::~WrenMethod()
	{
		wrenReleaseValue( m_vm, m_method );
		wrenReleaseValue( m_vm, m_variable );

		m_vm = nullptr;
		m_method = nullptr;
		m_variable = nullptr;
	}

	WrenEngine::WrenFunction* WrenEngine::WrenClass::FindFunction( const char* name )
	{
		for( WrenFunction& fn : Functions )
		{
			if( fn.Name == name )
				return &fn;
		}
		return nullptr;
	}

	WrenEngine::WrenEngine()
	{
		WrenConfiguration config;
		wrenInitConfiguration( &config );

		config.loadModuleFn = &LoadModuleCallback;
		config.bindForeignClassFn = &BindForeignClassCallback;
		config.bindForeignMethodFn = &BindForeignMethodCallback;
		config.writeFn = &WriteCallback;

		m_vm = wrenNewVM( &config );

		RegisterGlobalContext();

		m_activeEngines.Add( this );
	}

	WrenEngine::WrenEngine( WrenEngine&& other )
	{
		std::swap( m_vm, other.m_vm );
		std::swap( m_output, other.m_output );
		std::swap( m_classes, other.m_classes );
	}

	WrenEngine& WrenEngine::operator=( WrenEngine&& other )
	{
		std::swap( m_vm, other.m_vm );
		std::swap( m_output, other.m_output );
		std::swap( m_classes, other.m_classes );

		return *this;
	}

	WrenEngine::~WrenEngine()
	{
		m_activeEngines.RemoveItem( this );

		wrenFreeVM( m_vm );
	}

	void WrenEngine::RegisterGlobalContext()
	{
		WrenClass& classReg = m_classes.Allocate();
		classReg.Module = "dd";
		classReg.Name = "global";
		classReg.Type = nullptr;
	}
		
	void WrenEngine::SetOutput( String* output )
	{
		if( output != nullptr )
			m_output = new WriteStream( *output );
		else
			delete m_output;
	}

	void WrenEngine::Write( const char* message )
	{
		if( m_output == nullptr )
		{
			printf( "%s", message );
		}
		else
		{
			m_output->WriteFormat( "%s", message );
		}
	}

	WrenEngine::WrenClass* WrenEngine::FindClass( const char* module, const char* name ) const
	{
		for( WrenClass& entry : m_classes )
		{
			if( entry.Name == name )
				return &entry;
		}

		return nullptr;
	}

	bool WrenEngine::RunString( const char* script, String& output )
	{
		SetOutput( &output );

		WrenInterpretResult result = wrenInterpret( m_vm, script );
		
		switch( result )
		{
		case WREN_RESULT_COMPILE_ERROR:
			m_output->WriteFormat( "\nCOMPILE ERROR!" );
			return false;

		case WREN_RESULT_RUNTIME_ERROR:
			m_output->WriteFormat( "\nRUNTIME ERROR!" );
			return false;

		case WREN_RESULT_SUCCESS:
			break;
		}

		return true;
	}

	bool WrenEngine::RunModule( const char* module, String& output )
	{
		char* script = LoadModuleCallback( m_vm, module );
		
		SetOutput( &output );

		WrenInterpretResult result = wrenInterpretInModule( m_vm, module, script );

		switch( result )
		{
		case WREN_RESULT_COMPILE_ERROR:
			m_output->WriteFormat( "\nCOMPILE ERROR!" );
			return false;

		case WREN_RESULT_RUNTIME_ERROR:
			m_output->WriteFormat( "\nRUNTIME ERROR!" );
			return false;

		case WREN_RESULT_SUCCESS:
			break;
		}

		return true;
	}

	WrenMethod WrenEngine::GetMethod( const char* module, const char* variable, const char* method, uint arity )
	{
		wrenEnsureSlots( m_vm, 1 );
		wrenGetVariable( m_vm, module, variable, 0 );

		String128 fullSig( method );
		fullSig += "(";

		for( uint i = 0; i < arity; ++i )
		{
			if( i > 0 )
				fullSig += ",";
			fullSig += "_";
		}

		fullSig += ")";

		WrenValue* wrenVar = wrenGetSlotValue( m_vm, 0 );
		WrenValue* wrenMethod = wrenMakeCallHandle( m_vm, fullSig.c_str() );

		return WrenMethod( m_vm, wrenVar, wrenMethod, arity );
	}

	String256 WrenEngine::LoadModule( const char* module ) const
	{
		const char* path = "\\data\\wren\\";

		char current[256];
		_getcwd( current, 256 );

		String256 filename( current );
		filename += path;
		filename += module;
		filename += ".wren";

		std::FILE* file;
		fopen_s( &file, filename.c_str(), "rb" );

		if( file == nullptr )
			return String256();

		char buffer[256];
		String256 source;

		while( std::fgets( buffer, 256, file ) != nullptr )
		{
			source += buffer;
		}

		fclose( file );

		return source;
	}
}