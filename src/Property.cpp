//
// Property.h - A wrapper around a member that can be used to create a property editor.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#include "PCH.h"
#include "Property.h"

namespace dd
{
	Property::Property()
		: m_ptr( nullptr ),
		m_member( nullptr )
	{

	}

	Property::Property( Member& self, void* ptr )
		: m_member( &self ),
		m_ptr( ptr )
	{

	}

	Property::Property( const Property& other )
		: m_member( other.m_member ),
		m_ptr( other.m_ptr )
	{

	}

	Property::Property( Property&& other )
		: m_member( other.m_member ),
		m_ptr( other.m_ptr )
	{
		other.m_member = nullptr;
		other.m_ptr = nullptr;
	}

	Property::~Property()
	{
		m_ptr = nullptr;
		m_member = nullptr;
	}
}