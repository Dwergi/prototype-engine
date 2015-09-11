/*
Copyright (c) 2013 Randy Gaul http://RandyGaul.net

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Creator(s)    : Randy Gaul
Creation Date : Sat Jan 11 14:04:02 2014
File Name     : SELSignature.cpp
Purpose       : 
*/

#include "PrecompiledHeader.h"
#include "FunctionSignature.h"

namespace dd
{
	FunctionSignature::FunctionSignature()
		: m_ret( nullptr )
		, m_args( nullptr )
		, m_argCount( 0 )
	{
	}

	FunctionSignature::FunctionSignature( const FunctionSignature& rhs )
		: m_ret( rhs.m_ret )
		, m_args( rhs.m_args )
		, m_argCount( rhs.m_argCount )
	{
	}

	FunctionSignature& FunctionSignature::operator=( const FunctionSignature& rhs )
	{
		m_ret = rhs.m_ret;
		m_args = rhs.m_args;
		m_argCount = rhs.m_argCount;

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
