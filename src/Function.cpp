//
// FunctionSignature.h - A way to extract TypeInfo objects from function pointers.
// Copyright (C) Sebastian Nordgren
// Original version by Randy Gaul (RandyGaul.net)
// March 24th 2016
//

#include "PrecompiledHeader.h"
#include "Function.h"

namespace dd
{
	Function::Function()
		: m_sig()
		, m_callHelper( NULL )
		, m_context()
	{
	}

	Function::Function( const Function& rhs )
		: m_sig( rhs.m_sig )
		, m_callHelper( rhs.m_callHelper )
		, m_context( rhs.m_context )
	{
	}

	Function::Function( Function&& rhs )
		: m_sig( rhs.m_sig )
		, m_callHelper( rhs.m_callHelper )
		, m_context( rhs.m_context )
	{
	}

	Variable& Function::Context()
	{
		return m_context;
	}

	const Variable& Function::Context() const
	{
		return m_context;
	}

	void Function::Bind( Variable& context )
	{
		m_context = context;
	}

	bool Function::IsMethod() const
	{
		return m_sig.GetContext() ? true : false;
	}

	Function& Function::operator=( const Function& rhs )
	{
		m_sig = rhs.m_sig;
		m_callHelper = rhs.m_callHelper;

		return *this;
	}

	void Function::operator()( Variable& ret ) const
	{
		m_callHelper( &ret, m_context.Data(), NULL, 0 );
	}

	void Function::operator()() const
	{
		m_callHelper( NULL, m_context.Data(), NULL, 0 );
	}

	void Function::operator()( Variable& ret, Variable* args, uint argCount ) const
	{
		m_callHelper( &ret, m_context.Data(), args, argCount );
	}

	void Function::operator()( Variable& ret, Variable* args, uint argCount )
	{
		m_callHelper( &ret, m_context.Data(), args, argCount );
	}
}
