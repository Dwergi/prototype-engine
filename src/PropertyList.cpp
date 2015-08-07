//
// PropertyList.cpp - A list of properties that can be instantiated from a type and bound to an instance.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#include "PrecompiledHeader.h"
#include "PropertyList.h"

dd::PropertyListBase::PropertyListBase()
{
}

dd::PropertyListBase::~PropertyListBase()
{
}

void dd::PropertyListBase::Add( const Property& entry )
{
	m_properties.Add( entry );
}

dd::Property* dd::PropertyListBase::Find( const std::string& name )
{
	for( Property& prop : m_properties )
	{
		if( name == prop.GetName() )
			return &prop;

		if( name == prop.GetDisplayName() )
			return &prop;
	}

	return nullptr;
}

//
// Recursively add members to the property list.
//
void dd::PropertyListBase::AddMembers( dd::TypeInfo* typeInfo, uint offset )
{
	if( typeInfo == nullptr )
		return;

	const std::vector<MemberBase*>& members = typeInfo->GetMembers();
	for( MemberBase* member : members )
	{
		m_properties.Allocate( dd::Property( member, offset ) );

		dd::TypeInfo* nested = dd::TypeInfo::GetType( member->m_typeName );
		AddMembers( nested, offset + member->m_offset );
	}
}