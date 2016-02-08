//
// WrenEngine.h - Wrapper around the Wren engine.
// Copyright (C) Sebastian Nordgren 
// February 7th 2015
//

#pragma once

#include "wren\src\include\wren.h"

namespace dd
{
	class WrenEngine
	{
	public:

		WrenEngine();
		~WrenEngine();

		//
		// Register a method to an object.
		//
		template <typename FnType>
		void RegisterMethod( const char* name, const Function& method, FnType ptr );

		// 
		// Register a script object that is passed by reference exclusively.
		// 
		template <typename ObjType>
		void RegisterObject( const char* className );

		//
		// Register a script object that is passed by value.
		//
		template <typename ObjType>
		void RegisterStruct( const char* className );

		// 
		// Register a member for an object.
		//
		void RegisterMember( const char* className, const Member& member );

		template <typename FnType>
		void RegisterGlobalFunction( const char* name, const Function& function, FnType ptr, const char* explicit_sig = nullptr );

		void RegisterGlobalVariable( const char* name, const Variable& var );

		bool Evaluate( const String& script, String& output );

	private:

		static Vector<WrenEngine*> m_activeEngines;

		WrenVM* m_engine;

		static WrenForeignClassMethods	BindForeignClassCallback( WrenVM* vm, const char* module, const char* className );
		static WrenForeignMethodFn		BindForeignMethodCallback( WrenVM* vm, const char* module,	const char* className, bool isStatic, const char* signature );
		static char*					LoadModuleCallback( WrenVM* vm, const char* name );

		//
		// Find the engine that owns the given VM.
		//
		static WrenEngine*				FindEngine( WrenVM* vm );

		static void						CallFunction( WrenVM* vm );
	};
}