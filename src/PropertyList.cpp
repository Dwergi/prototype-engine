#include "PrecompiledHeader.h"
#include "PropertyList.h"

PropertyListBase::PropertyListBase()
{
}

PropertyListBase::~PropertyListBase()
{
}

void PropertyListBase::Add( const Property& entry )
{
	m_properties.push_back( entry );
}

Property* PropertyListBase::Find( const std::string& name )
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
void PropertyListBase::AddMembers( TypeInfo* typeInfo, uint offset )
{
	if( typeInfo == nullptr )
		return;

	const std::vector<MemberBase*>& members = typeInfo->GetMembers();
	for( MemberBase* member : members )
	{
		m_properties.emplace_back( Property( member, offset ) );

		TypeInfo* nested = TypeInfo::GetType( member->m_typeName );
		AddMembers( nested, offset + member->m_offset );
	}
}