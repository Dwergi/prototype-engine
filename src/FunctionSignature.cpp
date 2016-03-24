//
// FunctionSignature.h - A way to extract TypeInfo objects from function pointers.
// Copyright (C) Sebastian Nordgren
// Original version by Randy Gaul (RandyGaul.net)
// March 24th 2016
//

#include "PrecompiledHeader.h"
#include "FunctionSignature.h"

namespace dd
{
	FunctionSignature::FunctionSignature()
		: m_ret( nullptr )
		, m_args( nullptr )
		, m_argCount( 0 )
		, m_context( nullptr )
	{
	}

	FunctionSignature::FunctionSignature( const FunctionSignature& rhs )
		: m_ret( rhs.m_ret )
		, m_args( rhs.m_args )
		, m_argCount( rhs.m_argCount )
		, m_context( rhs.m_context )
	{
	}

	FunctionSignature& FunctionSignature::operator=( const FunctionSignature& rhs )
	{
		m_ret = rhs.m_ret;
		m_args = rhs.m_args;
		m_argCount = rhs.m_argCount;
		m_context = rhs.m_context;

		return *this;
	}

	FunctionSignature::FunctionSignature( void (*)() )
		: m_ret( nullptr )
		, m_args( nullptr )
		, m_argCount( 0 )
		, m_context( nullptr )
	{
	}

	uint FunctionSignature::ArgCount() const
	{
		return m_argCount;
	}

	const TypeInfo* FunctionSignature::GetRet() const
	{
		return m_ret;
	}

	const TypeInfo* FunctionSignature::GetArg( uint i ) const
	{
		return m_args[i];
	}

	const TypeInfo* FunctionSignature::GetContext() const
	{
		return m_context;
	}

	const FunctionSignature* Function::Signature() const
	{
		return &m_sig;
	}
}
