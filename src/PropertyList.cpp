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

void test_int_prop( Property* int_prop )
{
	int i = 5;
	int_prop->Set( i );

	i = 0;
	int_prop->Get( i );
	ASSERT( i == 5 );

	i = 0;
	int_prop->Get( i );
	ASSERT( i == 5 );

	i = 20;
	int_prop->Set( i );
	int_prop->Get( i );
	ASSERT( i == 20 );
}

void tests::PropertyTests()
{
	TestStruct::RegisterType();

	TestStruct test_struct;

	PropertyList<TestStruct> props( test_struct );
	Property* int_prop = props.Find( "Integer" );

	test_int_prop( int_prop );

	ASSERT( test_struct.Int == 20 );

	NestedStruct::RegisterType();

	NestedStruct nested;
	PropertyList<NestedStruct> nested_props( nested );

	Property* nested_int = nested_props.Find( "Integer" );
	test_int_prop( nested_int );

	Property* second_int = nested_props.Find( "SecondInt" );
	test_int_prop( second_int );

	ASSERT( nested.Struct.Int == 20 );
	ASSERT( nested.SecondInt == 20 );

	std::cout << "[PropertyEditor] Successfully tested property editor!" << std::endl;
}