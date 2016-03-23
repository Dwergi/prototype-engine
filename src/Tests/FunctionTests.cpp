//
// FunctionTests.h - Tests for Function.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "TestTypes.h"

TEST_CASE( "[TypeInfo] Find" )
{
	REGISTER_TYPE( Test::SimpleStruct );

	const dd::TypeInfo* type = GET_TYPE( Test::SimpleStruct );
	REQUIRE( type != nullptr );

	using namespace Test;

	const dd::TypeInfo* typeAlias = GET_TYPE( SimpleStruct );
	REQUIRE( typeAlias != nullptr );

	REQUIRE( type == typeAlias );
}

TEST_CASE( "[TypeInfo] Namespace" )
{
	REGISTER_TYPE( Test::SimpleStruct );

	const dd::TypeInfo* type = GET_TYPE( Test::SimpleStruct );
	REQUIRE( type != nullptr );

	REQUIRE( type->Namespace() == "Test" );
	REQUIRE( type->Name() == "SimpleStruct" );

	using namespace Test;

	const dd::TypeInfo* typeAlias = GET_TYPE( SimpleStruct );
	REQUIRE( typeAlias != nullptr );

	REQUIRE( typeAlias->Namespace() == "Test" );
	REQUIRE( typeAlias->Name() == "SimpleStruct" );
}