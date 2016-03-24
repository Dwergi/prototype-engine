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
		template <typename ObjType>
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

		bool Evaluate( const String& script, String& output );

		WrenVM* GetVM() const { return m_engine; }

		void SetOutput( String* output );
		void Write( const char* message );

	private:

		WrenVM* m_engine;
		WriteStream* m_output;

		Vector<WrenClass> m_classes;
		Vector<WrenVariable> m_variables;

		WrenClass* FindClass( const char* name ) const;
	};
}

#include "WrenEngine.inl"