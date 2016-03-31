#pragma once

#include "WrenArgument.h"
#include "WrenForeignFunction.h"

namespace dd
{
	namespace WrenInternal
	{
		template <typename T, typename... CtorArgs, std::size_t... Index>
		void Construct( WrenVM* vm, void* memory, std::index_sequence<Index...> )
		{
			new (memory) T( std::forward<CtorArgs>( WrenArgument<CtorArgs>::get( vm, Index + 1 ) )... );
		}

		template <typename T, typename... CtorArgs>
		void allocate( WrenVM* vm )
		{
			constexpr std::size_t ArgCount = sizeof...(CtorArgs);

			void* memory = wrenSetSlotNewForeign( vm, 0, 0, sizeof( T ) );
			Construct<T, CtorArgs...>( vm, memory, std::make_index_sequence<ArgCount>() );
		}

		template <typename T>
		void finalize( void* memory )
		{
			T* ptr = static_cast<T*>(memory);
			ptr->~T();
		}

		template <typename ObjType, typename PropType, PropType ObjType::* Member>
		void get( WrenVM* vm )
		{
			ObjType* object = static_cast<ObjType*>(wrenGetSlotForeign( vm, 0 ));
			WrenArgument<PropType>::set( vm, 1, object->*Member );
		}

		template <typename ObjType, typename PropType, PropType ObjType::* Member>
		void set( WrenVM* vm )
		{
			ObjType* object = static_cast<ObjType*>(wrenGetSlotForeign( vm, 0 ));
			object->*Member = WrenArgument<PropType>::get( vm, 1 );
		}

		template <typename T, T& Variable>
		void get_global( WrenVM* vm )
		{
			WrenArgument<T>::set( vm, 1, Variable );
		}

		template <typename T, T& Variable>
		void set_global( WrenVM* vm )
		{
			Variable = WrenArgument<T>::get( vm, 1 );
		}
	}

	template <typename T, typename... CtorArgs>
	void WrenEngine::RegisterType( bool byValue )
	{
		const TypeInfo* typeInfo = GET_TYPE( T );
		
		DD_ASSERT( FindClass( typeInfo->Namespace().c_str(), typeInfo->Name().c_str() ) == nullptr, "Can't register an object twice!" );
		
		WrenClass& classReg = m_classes.Allocate();
		classReg.Module = typeInfo->Namespace();
		classReg.Name = typeInfo->Name();
		classReg.Type = typeInfo;
		classReg.ForeignMethods.allocate = &WrenInternal::allocate<T, CtorArgs...>;
		classReg.ForeignMethods.finalize = &WrenInternal::finalize<T>;
	}

	template <typename FnType, FnType FunctionPtr >
	void WrenEngine::RegisterFunction( const char* name, const Function& function )
	{
		WrenClass* pClass { nullptr };

		if( function.Signature()->GetContext() == nullptr )
		{
			pClass = FindClass( "dd", "global" );
		}
		else
		{
			pClass = FindClass( function.Signature()->GetContext()->Namespace().c_str(), function.Signature()->GetContext()->Name().c_str() );
		}

		DD_ASSERT( pClass != nullptr );

		WrenFunction& fnReg = pClass->Functions.Allocate();
		fnReg.Name = name;
		fnReg.Function = &WrenInternal::WrenForeignFunction<FnType, FunctionPtr>::call;
	}

	template <typename ObjType, typename PropType, PropType ObjType::* Member >
	void WrenEngine::RegisterMember( const char* name )
	{
		const TypeInfo* typeInfo = GET_TYPE( ObjType );
		WrenClass* classReg = FindClass( typeInfo->Namespace().c_str(), typeInfo->Name().c_str() );

		DD_ASSERT( classReg != nullptr, "Class has not been registered yet!" );
		DD_ASSERT( classReg->FindFunction( name ) == nullptr, "Member %s has already been registered!", name );

		String128 fullName( name );

		// getter
		WrenFunction& getter = classReg->Functions.Allocate();
		getter.Name = fullName;
		getter.Function = &WrenInternal::get<ObjType, PropType, Member>;

		// setter
		fullName += "=(_)";

		WrenFunction& setter = classReg->Functions.Allocate();
		setter.Name = fullName;
		setter.Function = &WrenInternal::set<ObjType, PropType, Member>;
	}

	template <typename T, T& Variable>
	void WrenEngine::RegisterGlobalVariable( const char* name )
	{
		WrenClass* classReg = FindClass( "dd", "global" );
		DD_ASSERT( classReg != nullptr, "Global context not registered yet!" );
		DD_ASSERT( classReg->FindFunction( name ) == nullptr, "Variable %s has already been registered!", name );

		String128 fullName( name );
		
		WrenFunction& getter = classReg->Functions.Allocate();
		getter.Name = fullName;
		getter.Function = &WrenInternal::get_global<T, Variable>;

		WrenFunction& setter = classReg->Functions.Allocate();
		setter.Name = fullName;
		setter.Function = &WrenInternal::set_global<T, Variable>;
	}

	template <typename... Args, std::size_t... Index>
	void WrenMethodCallHelper( WrenVM* vm, std::tuple< Args... > args, std::index_sequence<Index...> )
	{
		ExpandType
		{
			0,
			(WrenArgument<std::tuple_element_t<Index, std::tuple<Args...>>>::set( vm, Index + 1, std::get<Index>( args ) ),
			0)...
		};
	}

	template <typename... Args>
	void WrenMethod::operator()( Args... args ) const
	{
		constexpr const std::size_t ArgCount = sizeof...(Args);
		DD_ASSERT( ArgCount == m_arity );

		wrenEnsureSlots( m_vm, m_arity + 1 );
		wrenSetSlotValue( m_vm, 0, m_variable );
		
		WrenMethodCallHelper( m_vm, std::make_tuple( args... ), std::make_index_sequence<ArgCount>() );

		wrenCall( m_vm, m_method );
	}
}