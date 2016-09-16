//
// AngelScriptEngine.h - Wrapper around the AngelScript engine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include <angelscript.h>

namespace dd
{
	class WriteStream;
	
	namespace ASInternal
	{
		template <typename T, bool>
		struct RegisterTypeForwarder;
	}

	//
	// A wrapper around a Wren method that is callable from C++.
	//
	class AngelScriptFunction
	{
	public:

		AngelScriptFunction() = delete;
		~AngelScriptFunction();

		AngelScriptFunction( AngelScriptFunction&& );
		AngelScriptFunction( const AngelScriptFunction& ) = delete;

		AngelScriptFunction& operator=( AngelScriptFunction&& );
		AngelScriptFunction& operator=( const AngelScriptFunction& ) = delete;

		bool Valid() const;
		void Invalidate();

		template <typename... Args>
		bool operator()( Args... );

		template <typename T>
		void Returned( T& ret ) const;

	private:
		friend class AngelScriptEngine;

		AngelScriptFunction( AngelScriptEngine*, asIScriptFunction* );

		void ReleaseContext();

		AngelScriptEngine* m_engine;
		asIScriptFunction* m_function;

		// context is created when the function is called and released on destruction or a new call
		asIScriptContext* m_context;
	};


	class AngelScriptEngine
	{
	public:
		AngelScriptEngine();
		~AngelScriptEngine();

		// disable copying
		AngelScriptEngine( const AngelScriptEngine& ) = delete;
		AngelScriptEngine& operator=( const AngelScriptEngine& ) = delete;

		// 
		// Register a type with the script system.
		// 
		template <typename T, bool byValue>
		void RegisterType();

		template <typename TClass, typename TProp, TProp TClass::* MemberPtr>
		void RegisterMember( const char* name, const TypeInfo* classType );

		template <typename FnType, FnType Fn>
		void RegisterFunction( const char* name );

		template <typename T, T& Variable>
		void RegisterGlobalVariable( const char* name );

		bool Evaluate( const String& script, String& output );

		bool LoadFile( const char* module, String& output );
		
		AngelScriptFunction* GetFunction( const char* module, const char* functionSig );

		asIScriptEngine* GetInternalEngine() const { return m_engine; }

		BASIC_TYPE( AngelScriptEngine )

	private:

		template <typename T, bool>
		friend struct ASInternal::RegisterTypeForwarder;

		asIScriptEngine* m_engine;
		WriteStream* m_output = nullptr;

		void SetOutput( String* output );

		void MessageCallback( const asSMessageInfo* msg, void* param );
		static String64 ReplacePointer( const char* typeName );

		static String256 GetFunctionSignatureString( const char* name, const Function& fn );

		template <typename FnType, FnType FnPtr>
		void RegisterMethod( const char* name, const Function& method );

		template <typename FnType, FnType FnPtr>
		void RegisterGlobalFunction( const char* name, const Function& method );

		template <typename T>
		void RegisterObject();

		template <typename T>
		void RegisterStruct();
	};
}

#include "AngelScriptEngine.inl"