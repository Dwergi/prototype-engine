//
// PropertyList.h - A list of properties that can be instantiated from a type and bound to an instance.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#pragma once

#include "Property.h"

namespace dd
{
/*
	class PropertyListBase
	{
	public: 
		PropertyListBase();
		virtual ~PropertyListBase();

		void Add( const Property& entry );
		Property* Find( const char* name );

	protected:
		dd::Vector<Property> m_properties;

		void AddMembers( TypeInfo* typeInfo, uint offset );
	};

	template<typename T>
	class PropertyList
		: public PropertyListBase
	{
	public:
		PropertyList( T& instance )
		{
			// recursively add members
			AddMembers( TypeInfo::GetType<T>(), 0 );

			// bind to this instance
			for( Property& prop : m_properties )
			{
				prop.Bind( instance );
			}
		}

		virtual ~PropertyList() {}
	};*/
}