//
// Property.h - A wrapper around a member that can be used to create a property editor.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#include "PrecompiledHeader.h"
#include "Property.h"

namespace dd
{
	Property::Property()
	{

	}

	Property::Property( const Function& get, const Function& set )
		: m_get( get ),
		m_set( set )
	{

	}
}

/*dd::Property::Property( dd::MemberBase* member, uint offset )
	: m_data( nullptr ),
	m_member( member ),
	m_offset( offset )
{
}

dd::Property::Property( Property&& entry )
	: m_data( entry.m_data ),
	m_member( entry.m_member ),
	m_offset( entry.m_offset )
{
}

dd::Property::~Property()
{
	m_data = nullptr;
}

const dd::StringBase& dd::Property::GetName() const
{
	return m_member->m_fieldName;
}

const dd::StringBase& dd::Property::GetDisplayName() const
{
	return m_member->m_friendlyName;
}*/