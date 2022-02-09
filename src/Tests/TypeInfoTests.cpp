//
// TypeInfoTests.h - Tests for TypeInfo.
// Copyright (C) Sebastian Nordgren 
// March 23rd 2016
//

#include "PCH.h"
#include "Tests.h"

#include "TestTypes.h"

namespace Test
{
	struct DerivedStruct : public SimpleStruct
	{
		uint DerivedThing { 0 };

		DD_BEGIN_CLASS(Test::DerivedStruct)
			DD_PARENT(Test::SimpleStruct);
			DD_MEMBER(DerivedThing);
		DD_END_CLASS()
	};
}

DD_CLASS_CPP(Test::SimpleStruct);
DD_CLASS_CPP(Test::DerivedStruct);

TEST_CASE("[TypeInfo] Find")
{
	const dd::TypeInfo* type = DD_FIND_TYPE(Test::SimpleStruct);
	REQUIRE(type != nullptr);

	using namespace Test;

	const dd::TypeInfo* typeAlias = DD_FIND_TYPE(Test::SimpleStruct);
	REQUIRE(typeAlias != nullptr);

	REQUIRE(type == typeAlias);
}

TEST_CASE("[TypeInfo] Namespace")
{
	const dd::TypeInfo* type = DD_FIND_TYPE(Test::SimpleStruct);
	REQUIRE(type != nullptr);

	REQUIRE(type->Namespace() == "Test");
	REQUIRE(type->Name() == "SimpleStruct");

	using namespace Test;

	const dd::TypeInfo* typeAlias = DD_FIND_TYPE(Test::SimpleStruct);
	REQUIRE(typeAlias != nullptr);

	REQUIRE(typeAlias->Namespace() == "Test");
	REQUIRE(typeAlias->Name() == "SimpleStruct");
}

TEST_CASE("[TypeInfo] Derived")
{
	const dd::TypeInfo* type = DD_FIND_TYPE(Test::DerivedStruct);
	REQUIRE(type != nullptr);

	REQUIRE(type->Namespace() == "Test");
	REQUIRE(type->Name() == "DerivedStruct");

	REQUIRE(type->GetMember("Int") != nullptr);
	REQUIRE(type->GetMethod("Multiply") != nullptr);
	REQUIRE(type->GetMember("DerivedThing") != nullptr);
}