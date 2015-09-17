//
// PropertyList.cpp - A list of properties that can be instantiated from a type and bound to an instance.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//
#include "PrecompiledHeader.h"
#include "PropertyList.h"

namespace 
{
	void* AddPointer( void* base, uint offset )
	{
		return (void*) (((byte*) base) + offset);
	}
}

namespace dd
{
	PropertyList::PropertyList( const PropertyList& other )
	{
		ASSERT( m_type == other.m_type );

		m_base = other.m_base;
		m_properties = other.m_properties;
	}

	PropertyList::~PropertyList()
	{
		m_base = nullptr;
		m_type = nullptr;
		m_properties.Clear();
	}

	Property* PropertyList::Find( const char* name )
	{
		for( Property& prop : m_properties )
		{
			if( prop.Name() == name )
				return &prop;
		}

		return nullptr;
	}

	//
	// Recursively add members to the property list.
	//
	void PropertyList::AddMembers( const TypeInfo* typeInfo, void* base )
	{
		if( typeInfo == nullptr )
			return;

		for( Member& member : typeInfo->GetMembers() )
		{
			void* ptr = AddPointer( base, member.Offset() );

			m_properties.Allocate( Property( member, ptr ) );

			AddMembers( member.Type(), ptr );
		}
	} 
}