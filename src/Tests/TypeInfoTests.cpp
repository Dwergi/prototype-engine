//
// TypeInfoTests.h - Tests for TypeInfo.
// Copyright (C) Sebastian Nordgren 
// March 23rd 2016
//

#include "PrecompiledHeader.h"
#include "catch2/catch.hpp"

#include "TestTypes.h"

namespace Test
{
	struct DerivedStruct : public SimpleStruct
	{
		uint DerivedThing;

		DD_BEGIN_TYPE( DerivedStruct )
			DD_PARENT( SimpleStruct )
			DD_MEMBER( DerivedStruct, DerivedThing )
		DD_END_TYPE
	};
}

TEST_CASE( "[TypeInfo] Find" )
{
	DD_REGISTER_TYPE( Test::SimpleStruct );

	const dd::TypeInfo* type = DD_TYPE( Test::SimpleStruct );
	REQUIRE( type != nullptr );

	using namespace Test;

	const dd::TypeInfo* typeAlias = DD_TYPE( SimpleStruct );
	REQUIRE( typeAlias != nullptr );

	REQUIRE( type == typeAlias );
}

TEST_CASE( "[TypeInfo] Namespace" )
{
	DD_REGISTER_TYPE( Test::SimpleStruct );

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
	DD_REGISTER_TYPE( Test::SimpleStruct );
	DD_REGISTER_TYPE( Test::DerivedStruct );

	const dd::TypeInfo* type = DD_TYPE( Test::DerivedStruct );
	REQUIRE( type != nullptr );

	REQUIRE( type->Namespace() == "Test" );
	REQUIRE( type->Name() == "DerivedStruct" );

	REQUIRE( type->GetMember( "Int" ) != nullptr );
	REQUIRE( type->GetMethod( "Multiply" ) != nullptr );
	REQUIRE( type->GetMember( "DerivedThing" ) != nullptr );
}