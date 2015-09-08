//
// PropertyList.h - A list of properties that can be instantiated from a type and bound to an instance.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#pragma once

#include "Property.h"
#include "TypeInfo.h"
#include "Vector.h"

namespace dd
{
	class PropertyList
	{
	public: 
		template<typename T>
		PropertyList( T& host )
			: m_base( &host )
		{
			AddMembers( GET_TYPE( T ), &host );
		}

		PropertyList( const PropertyList& other );
		~PropertyList();

		Property* Find( const char* name );

		void* Instance() { return m_base; }

	protected:
		Vector<Property> m_properties;
		const TypeInfo* m_type;
		void* m_base;

		void AddMembers( const TypeInfo* typeInfo, void* base );
	};
}