//
// Member.h - Encapsulates a member pointer and associates it with friendly names and stuff.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#include "PrecompiledHeader.h"
#include "Member.h"

namespace dd
{
	Member::Member()
		: m_typeInfo( nullptr ),
		m_offset( 0 )
	{
		
	}

	const TypeInfo* Member::Type() const
	{
		return m_typeInfo;
	}

	uint64 Member::Offset() const
	{
		return m_offset;
	}

	const String32& Member::Name() const
	{
		return m_name;
	}

	const TypeInfo* Member::Parent() const
	{
		return m_parent;
	}
}