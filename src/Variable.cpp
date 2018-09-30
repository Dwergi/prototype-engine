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
Creation Date : Wed Oct 09 23:58:36 2013
File Name     : SELVariable.cpp
Purpose       : 
*/

#include "PCH.h"
#include "Variable.h"

namespace dd
{
	Variable::Variable()
		: m_data( NULL )
		, m_typeInfo( NULL )
	{
	}

	Variable::Variable( const TypeInfo* typeInfo, void* data )
		: m_data( data )
		, m_typeInfo( typeInfo )
	{
	}

	Variable::Variable( Variable var, const Member& member )
	{
		m_data = PointerAdd( var.Data(), member.Offset() );
		m_typeInfo = member.Type();
	}

	Variable::Variable( const Variable& rhs )
	{
		m_data = rhs.m_data;
		m_typeInfo = rhs.m_typeInfo;
	}

	void* Variable::Data() const
	{
		return m_data;
	}

	const TypeInfo* Variable::Type() const
	{
		return m_typeInfo;
	}

	bool Variable::IsValid() const
	{
		return m_typeInfo != NULL && m_data != NULL;
	}

	void Variable::PlacementNew()
	{
		m_typeInfo->PlacementNew( m_data );
	}

	void Variable::PlacementDelete()
	{
		m_typeInfo->PlacementDelete( m_data );
	}

	void Variable::Delete()
	{
		m_typeInfo->PlacementDelete( m_data );
		m_typeInfo->Delete( m_data );
	}

	Variable& Variable::operator=( const Variable& rhs )
	{
		m_data = rhs.m_data;
		m_typeInfo = rhs.m_typeInfo;

		return *this;
	}
}
