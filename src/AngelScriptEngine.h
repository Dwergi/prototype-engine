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
		template <typename T>
		struct RegisterTypeForwarder;
	}

	//
	// A wrapper around a AngelScript method that is callable from C++.
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

		bool IsValid() const;

		template <typename... Args>
		bool operator()( Args... );

		template <typename T>
		void Returned( T& ret ) const;

	private:
		friend class AngelScriptEngine;
		friend class AngelScriptObject;

		AngelScriptFunction( AngelScriptEngine*, asIScriptFunction*, asIScriptObject* );

		void ReleaseContext();

		AngelScriptEngine* m_engine { nullptr };
		asIScriptFunction* m_function { nullptr };
		asIScriptObject* m_object { nullptr };

		// context is created when the function is called and released on destruction or a new call
		asIScriptContext* m_context { nullptr };
	};

	class AngelScriptObject
	{
	public:
		AngelScriptObject() = delete;
		~AngelScriptObject();

		AngelScriptObject( AngelScriptObject&& );
		AngelScriptObject( const AngelScriptObject& ) = delete;

		AngelScriptObject& operator=( AngelScriptObject&& );
		AngelScriptObject& operator=( const AngelScriptObject& ) = delete;

		bool IsValid() const;

		AngelScriptFunction* GetMethod( const char* signature );

	private:

		friend class AngelScriptEngine;

		AngelScriptObject( AngelScriptEngine*, asITypeInfo*, asIScriptObject* );

		void ReleaseObject();

		AngelScriptEngine* m_engine { nullptr };
		asIScriptObject* m_object { nullptr };
		asITypeInfo* m_typeInfo { nullptr };
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
		template <typename T>
		void RegisterType();

		template <typename TClass, typename TProp, TProp TClass::* MemberPtr>
		void RegisterMember( const char* name, const TypeInfo* classType );

		template <typename FnType, FnType Fn>
		void RegisterFunction( const char* name );

		template <typename T, T& Variable>
		void RegisterGlobalVariable( const char* name );

		bool Evaluate( const String& script, String& output );

		bool LoadFile( const char* module, String& output );
		
		bool IsModuleLoaded( const char* module ) const;

		AngelScriptFunction* GetFunction( const char* module, const char* functionSig );

		AngelScriptObject* GetScriptObject( const char* module, const char* className );

		asIScriptEngine* GetInternalEngine() const { return m_engine; }

		DD_BASIC_TYPE( AngelScriptEngine )

	private:

		template <typename T>
		friend struct ASInternal::RegisterTypeForwarder;

		asIScriptEngine* m_engine;
		WriteStream* m_output = nullptr;

		void SetOutput( String* output );

		void MessageCallback( const asSMessageInfo* msg );
		static String64 ReplacePointer( const char* typeName );

		static String256 GetFunctionSignatureString( const char* name, const Function& fn );

		template <typename FnType, FnType FnPtr>
		void RegisterMethod( const char* name, const Function& method );

		template <typename FnType, FnType FnPtr>
		void RegisterGlobalFunction( const char* name, const Function& method );

		template <typename T>
		void RegisterObject();

		String256 LoadSource( const char* module ) const;
	};
}

#include "AngelScriptEngine.inl"