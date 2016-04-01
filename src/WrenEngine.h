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

	//
	// A wrapper around a Wren method that is callable from C++.
	//
	class WrenMethod
	{
	public:

		WrenMethod() = delete;
		~WrenMethod();

		WrenMethod( WrenMethod&& );
		WrenMethod( const WrenMethod& ) = delete;

		WrenMethod& operator=( WrenMethod&& );
		WrenMethod& operator=( const WrenMethod& ) = delete;

		template <typename... Args>
		void operator()( Args... ) const;

	private:
		friend class WrenEngine;

		WrenMethod( WrenVM*, WrenValue* variable, WrenValue* method, uint arity );

		WrenVM* m_vm;
		WrenValue* m_variable;
		WrenValue* m_method;
		uint m_arity;
	};

	class WrenEngine
	{
	public:

		WrenEngine();
		WrenEngine( WrenEngine&& other );
		~WrenEngine();

		WrenEngine& operator=( WrenEngine&& other );

		WrenEngine( const WrenEngine& ) = delete;
		WrenEngine& operator=( const WrenEngine& ) = delete;

		// 
		// Register a script object that is passed by reference exclusively.
		// @byValue - Whether this object can be passed by value or not.
		// 
		template <typename T, typename... CtorArgs>
		void RegisterType( bool byValue );

		// 
		// Register a script-accessible member for an object.
		//
		template <typename ObjType, typename PropType, typename PropType ObjType::* Member>
		void RegisterMember( const char* name );

		//
		// Register a function to be callable from script.
		// It doesn't matter if it's a method or a global function, 
		// the Function object includes the required info to register either.
		//
		template <typename FnType, FnType FunctionPtr>
		void RegisterFunction( const char* name, const Function& function );

		//
		// Register a global variable to be script-accessible.
		// In Wren this means that we register getters and setters in the dd.global class.
		//
		template <typename T, T& Variable>
		void RegisterGlobalVariable( const char* name );

		//
		// Evaluate a script snippet, and place the output into the output string.
		// Essentially a REPL interface.
		// Returns true if the snippet executed without errors.
		//
		bool RunString( const char* script, String& output );

		//
		// Evaluate a module and place the output into the output string.
		// Loads the module off disk if desired.
		// Returns true if the snippet executed without errors.
		//
		bool RunModule( const char* module, String& output );

		//
		// Bind a Wren method from the given module, called on the given variable with the given name and arity.
		//
		WrenMethod GetMethod( const char* module, const char* variable, const char* method, uint arity );

	private:

		struct WrenFunction
		{
			SharedString Name;
			WrenForeignMethodFn Function;
		};

		struct WrenClass
		{
			SharedString Module;
			SharedString Name;

			const TypeInfo* Type;
			Vector<WrenFunction> Functions;
			WrenForeignClassMethods ForeignMethods;

			WrenFunction* FindFunction( const char* name );
		};

		WrenVM* m_vm;
		WriteStream* m_output;
		Vector<WrenClass> m_classes;

		// Internal helper for finding the engine that is registered with the given VM.
		static WrenEngine* FindEngine( WrenVM* vm );

		// Friend functions for Wren internals.
		friend WrenForeignClassMethods BindForeignClassCallback( WrenVM*, const char*, const char* );
		friend WrenForeignMethodFn BindForeignMethodCallback( WrenVM*, const char*, const char*, bool, const char* );
		friend char* LoadModuleCallback( WrenVM*, const char* );
		friend void WriteCallback( WrenVM*, const char* );

		void RegisterGlobalContext();

		void SetOutput( String* output );
		void Write( const char* message );

		String256 LoadModule( const char* module ) const;
		WrenClass* FindClass( const char* module, const char* name ) const;
	};
}

#include "WrenEngine.inl"