#pragma once

namespace dd
{
	template <typename ObjType, typename... CtorArgs, std::size_t... Index>
	void Construct( WrenVM* vm, void* memory, std::index_sequence<Index...> )
	{
		new (memory) ObjType { ExpandType { std::tuple_element_t<Index, std::tuple<CtorArgs...>> }... };
	}

	template <typename ObjType, typename... CtorArgs>
	void allocate( WrenVM* vm )
	{
		constexpr std::size_t ArgCount = sizeof...( CtorArgs );

		void* memory = wrenSetSlotNewForeign( vm, 0, 0, sizeof( ObjType ) );
		Construct<ObjType, CtorArgs...>( vm, memory, std::make_index_sequence<CtorArgs>() );
	}

	template <typename ObjType>
	void finalize( void* memory )
	{
		ObjType* ptr = static_cast<ObjType*>(memory);
		ptr->~ObjType();
	}

	template <typename ObjType, typename... CtorArgs>
	void WrenEngine::RegisterObjectType( bool byValue )
	{
		const TypeInfo* typeInfo = GET_TYPE( ObjType );
		
		DD_ASSERT( FindClass( typeInfo->Name() ) == nullptr, "Can't register an object twice!" );
		
		WrenClass& classReg = m_classes.Allocate();
		classReg.Name = typeInfo->Name();
		classReg.Type = typeInfo;
		classReg.ForeignMethods.allocate = &allocate<ObjType, CtorArgs...>;
		classReg.ForeignMethods.finalize = &finalize<ObjType>;
	}

	template <typename FnType>
	void WrenEngine::RegisterFunction( const char* name, const Function& function, FnType ptr )
	{
		WrenClass* pClass { nullptr };

		if( function.Context() == nullptr )
		{
			pClass = FindClass( "dd" );
		}
		else
		{
			pClass = FindClass( function.Context()->Name() );
		}

		DD_ASSERT( pClass != nullptr );

		WrenFunction& fnReg = pClass->Functions.Allocate();
		fnReg.Name = name;
		fnReg.Function = function;
	}
}