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

	WrenForeignClassMethods BindForeignClassCallback( WrenVM* vm, const char* module, const char* className )
	{
		WrenForeignClassMethods ret;
		ret.allocate = nullptr;
		ret.allocate = nullptr;

		WrenEngine* engine = FindEngine( vm );

		if( engine == nullptr )
			return ret;

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

		m_engine = wrenNewVM( &config );

		m_activeEngines.Add( this );
	}

	WrenEngine::~WrenEngine()
	{
		m_activeEngines.RemoveItem( this );

		wrenFreeVM( m_engine );
	}
	
	void WrenEngine::CallFunction( WrenVM* vm )
	{
		WrenEngine* engine = FindEngine( vm );

		if( engine == nullptr )
			return;
	}
}