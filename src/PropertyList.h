#pragma once

#include "Property.h"

namespace dd
{
	class PropertyListBase
	{
	public: 
		PropertyListBase();
		virtual ~PropertyListBase();

		void Add( const Property& entry );
		Property* Find( const std::string& name );

	protected:
		std::vector<Property> m_properties;

		void AddMembers( TypeInfo* typeInfo, size_t offset );
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
	};
}