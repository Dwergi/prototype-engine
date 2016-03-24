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
		// @byValue - Whether this object can be passed by value or not.
		// 
		template <typename ObjType>
		void RegisterObject( bool byValue );

		// 
		// Register a script-accessible member for an object.
		//
		void RegisterMember( const Member& member );

		//
		// Register a 
		//
		template <typename FnType>
		void RegisterGlobalFunction( const char* name, const Function& function, FnType ptr );

		void RegisterGlobalVariable( const char* name, const Variable& var );

		bool Evaluate( const String& script, String& output );

		WrenVM* GetVM() const { return m_engine; }

		void SetOutput( String* output );
		void Write( const char* message );

	private:

		WrenVM* m_engine;
		WriteStream* m_output;
	};
}

#include "WrenEngine.inl"