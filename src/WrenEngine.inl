#pragma once

namespace dd
{
	template <typename ObjType>
	void WrenEngine::RegisterObjectType( bool byValue )
	{
		const TypeInfo* typeInfo = GET_TYPE( ObjType );
		
		DD_ASSERT( FindClass( typeInfo->Name() ) == nullptr, "Can't register an object twice!" );
		
		WrenClass& classReg = m_classes.Allocate();
		classReg.Name = typeInfo->Name();
		classReg.Type = typeInfo;
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