//
// AngelScriptEngine.inl - Inline functions for AngelScriptEngine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "ASInternal.h"

namespace dd
{
	namespace ASInternal
	{
		template <typename T>
		T* Factory()
		{
			T* ptr = new T();
			ptr->m_refCount.Init( ptr, GET_TYPE( T ) );
			return ptr;
		}

		template <typename T>
		void Construct( T* memory )
		{
			new (memory) T();
		}

		template <typename T>
		void CopyConstruct( const T& other, T* self )
		{
			new (self) T( other );
		}

		template <typename T>
		void Destruct( T* memory )
		{
			memory->~T();
		}

		//
		// Register type T to be handled by value or by reference.
		//
		template <typename T, bool>
		struct RegisterTypeForwarder
		{
			static void Register( dd::AngelScriptEngine* ptr )
			{
				ptr->RegisterObject<T>();
			}
		};

		template <typename T>
		struct RegisterTypeForwarder<T, true>
		{
			static void Register( dd::AngelScriptEngine* ptr )
			{
				ptr->RegisterStruct<T>();
			}
		};

		template <typename... Args, std::size_t... Index>
		void MethodCallHelper( asIScriptContext* context, std::tuple<Args...> args, std::index_sequence<Index...> )
		{
			ExpandType
			{
				0, (
					ASInternal::ASArgument<std::tuple_element_t<Index, std::tuple<Args...>>>::set( context, Index, std::get<Index>( args ) ),
				0)...
			};
		}
	}

	template <typename... Args>
	bool AngelScriptFunction::operator()( Args... args )
	{
		constexpr const std::size_t ArgCount = sizeof...(Args);
		DD_ASSERT( ArgCount == m_function->GetParamCount() );

		ReleaseContext();

		m_context = m_engine->GetInternalEngine()->CreateContext();
		m_context->Prepare( m_function );

		if( m_object != nullptr )
		{
			m_context->SetObject( m_object );
		}

		ASInternal::MethodCallHelper( m_context, std::make_tuple( args... ), std::make_index_sequence<ArgCount>() );

		int r = m_context->Execute();
		if( r != asEXECUTION_FINISHED )
		{
			ReleaseContext();
			return false;
		}

		return true;
	}

	template <typename T>
	void AngelScriptFunction::Returned( T& ret ) const
	{
		if( m_context != nullptr )
		{
			ASInternal::ReturnValue<T>::Get( m_context, ret );
		}
	}

	template <typename T, bool byValue>
	void AngelScriptEngine::RegisterType()
	{
		ASInternal::RegisterTypeForwarder<T, byValue>::Register( this );
	}
	
	template <typename T>
	void AngelScriptEngine::RegisterObject()
	{
		const TypeInfo* typeInfo = GET_TYPE( T );
		DD_ASSERT( typeInfo->IsRegistered() );

		int res = m_engine->RegisterObjectType( typeInfo->Name().c_str(), 0, asOBJ_REF );
		DD_ASSERT( res >= 0 );

		String32 strSig( typeInfo->Name().c_str() );
		strSig += "@ Factory()";

		res = m_engine->RegisterObjectBehaviour( typeInfo->Name().c_str(), asBEHAVE_FACTORY, strSig.c_str(), asFUNCTION( ASInternal::Factory<T> ), asCALL_CDECL );
		DD_ASSERT( res >= 0 );

		res = m_engine->RegisterObjectBehaviour( typeInfo->Name().c_str(), asBEHAVE_ADDREF, "void Increment()", asMETHOD( RefCounter, Increment ), asCALL_THISCALL );
		DD_ASSERT( res >= 0 );

		res = m_engine->RegisterObjectBehaviour( typeInfo->Name().c_str(), asBEHAVE_RELEASE, "void Decrement()", asMETHOD( RefCounter, Decrement ), asCALL_THISCALL );
		DD_ASSERT( res >= 0 );
	}

	template <typename T>
	void AngelScriptEngine::RegisterStruct()
	{
		const TypeInfo* typeInfo = GET_TYPE( T );
		DD_ASSERT( typeInfo->IsRegistered() );

		String64 objType( ReplacePointer( typeInfo->Name().c_str() ) );

		int res = m_engine->RegisterObjectType( objType.c_str(), sizeof( T ), asOBJ_VALUE | asGetTypeTraits<T>() );
		DD_ASSERT( res >= 0, "Failed to register struct '%s'!", typeInfo->Name().c_str() );

		res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_CONSTRUCT, "void Construct()", asFUNCTION( ASInternal::Construct<T> ), asCALL_CDECL_OBJLAST );
		DD_ASSERT( res >= 0 );

		String128 copyConstructSig;
		copyConstructSig += "void CopyConstruct(const ";
		copyConstructSig += objType.c_str();
		copyConstructSig += "& in)";

		res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_CONSTRUCT, copyConstructSig.c_str(), asFUNCTION( ASInternal::CopyConstruct<T> ), asCALL_CDECL_OBJLAST );
		DD_ASSERT( res >= 0 );

		res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_DESTRUCT, "void Destruct()", asFUNCTION( ASInternal::Destruct<T> ), asCALL_CDECL_OBJLAST );
		DD_ASSERT( res >= 0 );

		String128 opAssignSig;
		opAssignSig += objType.c_str();
		opAssignSig += "& opAssign(const ";
		opAssignSig += objType.c_str();
		opAssignSig += "& in)";

		res = m_engine->RegisterObjectMethod( objType.c_str(), opAssignSig.c_str(), asMETHODPR( T, operator=, (const T&), T& ), asCALL_THISCALL );
		DD_ASSERT( res >= 0 );
	}

	template <typename TClass, typename TProp, TProp TClass::* MemberPtr>
	void AngelScriptEngine::RegisterMember( const char* name, const TypeInfo* classType )
	{
		const TypeInfo* propType = GET_TYPE( TProp );

		String128 signature;
		signature += ReplacePointer( propType->Name().c_str() );
		signature += " ";
		signature += name;

		uint64 offset = (uint64) (&(((TClass*) nullptr)->*MemberPtr));

		int res = m_engine->RegisterObjectProperty( classType->Name().c_str(), signature.c_str(), (int) offset );
		DD_ASSERT( res >= 0 );
	}

	template <typename T, T& Variable>
	void AngelScriptEngine::RegisterGlobalVariable( const char* name )
	{
		const TypeInfo* typeInfo = GET_TYPE( T );
		DD_ASSERT( typeInfo != nullptr );

		String128 signature;
		signature += ReplacePointer( typeInfo->Name().c_str() );
		signature += " ";
		signature += name;

		int res = m_engine->RegisterGlobalProperty( signature.c_str(), &Variable );
		DD_ASSERT( res >= 0 );
	}

	template <typename FnType, FnType FnPtr>
	void AngelScriptEngine::RegisterGlobalFunction( const char* name, const Function& function )
	{
		String256 signature = GetFunctionSignatureString( name, function );

		asSFuncPtr ptr = asSMethodPtr<sizeof( FnType )>::Convert( FnPtr );
		ptr.flag = 2; // global calling convention, because fuck you AngelScript
		int res = m_engine->RegisterGlobalFunction( signature.c_str(), ptr, asCALL_CDECL );
		DD_ASSERT( res >= 0, "Failed to register global function \'%s\'!", signature.c_str() );
	}

	template <typename FnType, FnType FnPtr>
	void AngelScriptEngine::RegisterMethod( const char* method_name, const Function& method )
	{
		String256 signature( GetFunctionSignatureString( method_name, method ) );

		String64 className( ReplacePointer( method.Signature()->GetContext()->Name().c_str() ) );

		int res = m_engine->RegisterObjectMethod( className.c_str(), signature.c_str(), asSMethodPtr<sizeof(FnType)>::Convert( FnPtr ), asCALL_THISCALL );
		DD_ASSERT( res >= 0, "Failed to register method \'%s\' for class \'%s\'!", signature.c_str(), className.c_str() );
	}

	template <typename FnType, FnType FnPtr>
	void AngelScriptEngine::RegisterFunction( const char* name )
	{
		Function function( BuildFunction<FnType, FnPtr>( FnPtr ) );

		if( function.Signature()->GetContext() == nullptr )
		{
			RegisterGlobalFunction<FnType, FnPtr>( name, function );
		}
		else
		{
			RegisterMethod<FnType, FnPtr>( name, function );
		}
	}
}