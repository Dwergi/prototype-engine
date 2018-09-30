//
// TypeInfoTests.h - Tests for TypeInfo.
// Copyright (C) Sebastian Nordgren 
// March 23rd 2016
//

#include "PCH.h"
#include "catch2/catch.hpp"

#include "TestTypes.h"

namespace Test
{
	struct DerivedStruct : public SimpleStruct
	{
		uint DerivedThing;

		DD_CLASS( dd::DerivedStruct )
		{
			DD_PARENT( dd::SimpleStruct );
			DD_MEMBER( dd::DerivedStruct, DerivedThing );
		}
	};
}

TEST_CASE( "[TypeInfo] Find" )
{
	DD_REGISTER_CLASS( Test::SimpleStruct );

	const dd::TypeInfo* type = DD_TYPE( Test::SimpleStruct );
	REQUIRE( type != nullptr );

	using namespace Test;

	const dd::TypeInfo* typeAlias = DD_TYPE( SimpleStruct );
	REQUIRE( typeAlias != nullptr );

	REQUIRE( type == typeAlias );
}

TEST_CASE( "[TypeInfo] Namespace" )
{
	DD_REGISTER_CLASS( Test::SimpleStruct );

	const dd::TypeInfo* type = DD_TYPE( Test::SimpleStruct );
	REQUIRE( type != nullptr );

	REQUIRE( type->Namespace() == "Test" );
	REQUIRE( type->Name() == "SimpleStruct" );

	using namespace Test;

	const dd::TypeInfo* typeAlias = DD_TYPE( SimpleStruct );
	REQUIRE( typeAlias != nullptr );

	REQUIRE( typeAlias->Namespace() == "Test" );
	REQUIRE( typeAlias->Name() == "SimpleStruct" );
}

TEST_CASE( "[TypeInfo] Derived" )
{
	DD_REGISTER_CLASS( Test::SimpleStruct );
	DD_REGISTER_CLASS( Test::DerivedStruct );

	const dd::TypeInfo* type = DD_TYPE( Test::DerivedStruct );
	REQUIRE( type != nullptr );

	REQUIRE( type->Namespace() == "Test" );
	REQUIRE( type->Name() == "DerivedStruct" );

	REQUIRE( type->GetMember( "Int" ) != nullptr );
	REQUIRE( type->GetMethod( "Multiply" ) != nullptr );
	REQUIRE( type->GetMember( "DerivedThing" ) != nullptr );
}