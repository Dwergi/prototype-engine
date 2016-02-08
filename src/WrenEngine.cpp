//
// WrenEngine.cpp - Wrapper around the Wren script engine.
// Copyright (C) Sebastian Nordgren 
// February 7th 2015
//

#include "PrecompiledHeader.h"
#include "WrenEngine.h"

namespace dd
{
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

	WrenEngine* WrenEngine::FindEngine( WrenVM* vm )
	{
		for( WrenEngine* engine : m_activeEngines )
		{
			if( engine->m_engine == vm )
				return engine;
		}

		return nullptr;
	}

	void WrenEngine::CallFunction( WrenVM* vm )
	{
		WrenEngine* engine = FindEngine( vm );

		if( engine == nullptr )
			return;
	}

	WrenForeignClassMethods WrenEngine::BindForeignClassCallback( WrenVM* vm, const char* module, const char* className )
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

	WrenForeignMethodFn	WrenEngine::BindForeignMethodCallback( WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature )
	{
		WrenEngine* engine = FindEngine( vm );

		if( engine == nullptr )
			return nullptr;

		// TODO
		return nullptr;
	}

	char* WrenEngine::LoadModuleCallback( WrenVM* vm, const char* name )
	{
		WrenEngine* engine = FindEngine( vm );

		if( engine == nullptr )
			return nullptr;

		// TODO
		return nullptr;
	}
}