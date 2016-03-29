//
// WrenEngine.h - Wrapper around the Wren engine.
// Copyright (C) Sebastian Nordgren 
// February 7th 2015
//

#pragma once

#include "wren/src/include/wren.h"

namespace dd
{
	class WriteStream;

	struct WrenFunction
	{
		SharedString Name;
		Function Function;
	};

	struct WrenClass
	{
		SharedString Name;
		const TypeInfo* Type;
		Vector<WrenFunction> Functions;
		Vector<Member> Members;
		WrenForeignClassMethods ForeignMethods;
	};

	struct WrenVariable
	{
		SharedString Name;
		Variable Variable;
	};

	class WrenEngine
	{
	public:

		WrenEngine();
		~WrenEngine();

		// 
		// Register a script object that is passed by reference exclusively.
		// @byValue - Whether this object can be passed by value or not.
		// 
		template <typename ObjType, typename... CtorArgs>
		void RegisterObjectType( bool byValue );

		// 
		// Register a script-accessible member for an object.
		//
		void RegisterMember( const Member& member );

		//
		// Register a function to be callable from script.
		// It doesn't matter if it's a method or a global function, 
		// the Function object includes the required info to register either.
		//
		template <typename FnType>
		void RegisterFunction( const char* name, const Function& function, FnType ptr );

		//
		// Register an instance of a global variable to be script-accessible. 
		// This means that 
		//
		void RegisterGlobalVariable( const char* name, const Variable& var );

		//
		// Evaluate a script snippet, and place the output into the output string.
		// Essentially a REPL interface.
		// Returns true if the snippet executed without errors.
		//
		bool Evaluate( const String& script, String& output );

	private:

		WrenVM* m_vm;
		WriteStream* m_output;

		Vector<WrenClass> m_classes;
		Vector<WrenVariable> m_variables;

		// Internal helper for finding the engine that is registered with the given VM.
		static WrenEngine* FindEngine( WrenVM* vm );

		// Friend functions for Wren internals.
		friend WrenForeignClassMethods BindForeignClassCallback( WrenVM* vm, const char* module, const char* className );
		friend WrenForeignMethodFn BindForeignMethodCallback( WrenVM*, const char*, const char*, bool, const char* );
		friend char* LoadModuleCallback( WrenVM*, const char* );
		friend void WriteCallback( WrenVM* vm, const char* msg );

		void SetOutput( String* output );
		void Write( const char* message );

		char* LoadModule( const char* name ) const;

		WrenClass* FindClass( const char* name ) const;
	};
}

#include "WrenEngine.inl"