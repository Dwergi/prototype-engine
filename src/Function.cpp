/*
Copyright (c) 2013-2014 Randy Gaul http://RandyGaul.net

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
Creation Date : Wed Nov 27 01:06:27 2013
File Name     : SELFunction.cpp
Purpose       : 
*/

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
		, m_context()
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

	Function::Function( 
		void (*fn)(),
		void (*helper)( Variable* , void* , Variable* , unsigned )
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	void Function::operator()( Variable& ret ) const
	{
		m_callHelper( &ret, m_context.GetData(), NULL, 0 );
	}

	void Function::operator()() const
	{
		m_callHelper( NULL, m_context.GetData(), NULL, 0 );
	}

	void Function::operator()( Variable& ret, Variable* args, unsigned argCount ) const
	{
		m_callHelper( &ret, m_context.GetData(), args, argCount );
	}

	void Function::operator()( Variable& ret, Variable* args, unsigned argCount )
	{
		m_callHelper( &ret, m_context.GetData(), args, argCount );
	}
}
