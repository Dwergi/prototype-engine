//
// WrenEngine.cpp - Wrapper around the Wren script engine.
// Copyright (C) Sebastian Nordgren 
// February 7th 2015
//

#include "PrecompiledHeader.h"
#include "WrenEngine.h"

namespace dd
{
	static Vector<WrenEngine*> m_activeEngines;

	//
	// Find the engine that owns the given VM.
	//
	WrenEngine* FindEngine( WrenVM* vm )
	{
		for( WrenEngine* engine : m_activeEngines )
		{
			if( engine->GetVM() == vm )
				return engine;
		}

		return nullptr;
	}

	//
	// Callback from Wren when something like print() is called that should write to output.
	//
	void WriteCallback( WrenVM* vm, const char* msg )
	{
		WrenEngine* engine = FindEngine( vm );
		if( engine == nullptr )
			return;

		engine->Write( msg );
	}

	WrenForeignClassMethods BindForeignClassCallback( WrenVM* vm, const char* module, const char* className )
	{
		WrenForeignClassMethods ret;
		ret.allocate = nullptr;
		ret.finalize = nullptr;

		WrenEngine* engine = FindEngine( vm );
		if( engine == nullptr )
			return ret;

		String128 fullName( module );
		fullName += "::";
		fullName += className;

		const TypeInfo* typeInfo = GET_TYPE_STR( fullName );
		if( typeInfo == nullptr )
			return ret;

		void* newPtr = wrenSetSlotNewForeign( vm, 0, 0, typeInfo->Size() );
		typeInfo->PlacementNew( newPtr );

		// TODO
		return ret;
	}

	WrenForeignMethodFn	BindForeignMethodCallback( WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature )
	{
		WrenEngine* engine = FindEngine( vm );

		if( engine == nullptr )
			return nullptr;

		// TODO
		return nullptr;
	}

	char* LoadModuleCallback( WrenVM* vm, const char* name )
	{
		WrenEngine* engine = FindEngine( vm );

		if( engine == nullptr )
			return nullptr;

		// TODO
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

		m_engine = wrenNewVM( &config );

		m_activeEngines.Add( this );
	}

	WrenEngine::~WrenEngine()
	{
		m_activeEngines.RemoveItem( this );

		wrenFreeVM( m_engine );
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
			printf( "%s\n\n", message );
		}
		else
		{
			m_output->WriteFormat( "%s\n", message );
		}
	}
}