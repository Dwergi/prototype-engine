#include "PrecompiledHeader.h"
#include "Property.h"

Property::Property( MemberBase* member, uint offset )
	: m_data( nullptr ),
	m_member( member ),
	m_offset( offset )
{
}

Property::Property( Property&& entry )
	: m_data( entry.m_data ),
	m_member( entry.m_member ),
	m_offset( entry.m_offset )
{
}

Property::~Property()
{
	m_data = nullptr;
}

const std::string& Property::GetName() const
{
	return m_member->m_fieldName;
}

const std::string& Property::GetDisplayName() const
{
	return m_member->m_friendlyName;
}