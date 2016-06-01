//
// AngelScriptEngine.h - Wrapper around the AngelScript engine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include <angelscript.h>

namespace dd
{
	namespace ASInternal
	{
		template <typename T, bool>
		struct RegisterTypeForwarder;
	}

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
		bool RunFunction( const char* module, const String& functionSig, String& output );

		asIScriptEngine* GetInternalEngine() const { return m_engine; }

		BASIC_TYPE( AngelScriptEngine )

	private:

		template <typename T, bool>
		friend struct ASInternal::RegisterTypeForwarder;

		asIScriptEngine* m_engine;

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