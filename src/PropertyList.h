#pragma once

#include "Property.h"

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

namespace tests
{
	struct TestStruct
	{
		int Int;
		std::string Str;
		float Flt;

		BEGIN_MEMBERS( tests::TestStruct )
			MEMBER( tests::TestStruct, int, Int, "Integer" );
		MEMBER( tests::TestStruct, std::string, Str, "String" );
		MEMBER( tests::TestStruct, float, Flt, "Float" );
		END_MEMBERS
	};

	struct NestedStruct
	{
		TestStruct Struct;
		int SecondInt;

		BEGIN_MEMBERS( tests::NestedStruct )
			MEMBER( tests::NestedStruct, tests::TestStruct, Struct, "Struct" );
		MEMBER( tests::NestedStruct, int, SecondInt, "Second Integer" );
		END_MEMBERS
	};

	void PropertyTests();
}