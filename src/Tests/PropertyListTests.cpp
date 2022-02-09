//
// PropertyListTests.cpp - Tests for PropertyList.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#include "PCH.h"
#include "Tests.h"

#include "PropertyList.h"
#include "TransformComponent.h"

#include "TestTypes.h"

using namespace dd;

DD_CLASS_CPP(Test::SimpleStruct);
DD_CLASS_CPP(Test::NestedStruct);

void test_int_prop(Property& int_prop)
{
	int i = 5;
	int_prop.SetFromVariable(Variable(i));

	i = 0;
	Variable var = int_prop.GetVariable();
	REQUIRE(var.Get<int>() == 5);

	i = 0;
	var = int_prop.GetVariable();
	REQUIRE(var.Get<int>() == 5);

	i = 20;
	int_prop.SetFromVariable(Variable(i));
	var = int_prop.GetVariable();
	REQUIRE(var.Get<int>() == 20);

	int_prop.Set(111);
	REQUIRE(var.Get<int>() == 111);
}

TEST_CASE("[Property] Integer properties")
{
	SECTION("Simple")
	{
		Test::SimpleStruct test_struct;
		test_struct.Str = "Test";
		test_struct.Vec.Add(5);
		test_struct.Vec.Add(10);

		PropertyList props(test_struct);

		const TypeInfo* type = DD_FIND_TYPE(Test::SimpleStruct);
		REQUIRE(props.Type() == type);
		REQUIRE(props.Size() == type->Members().Size());

		std::optional<Property> int_prop = props.Find("Int");
		REQUIRE(int_prop);

		test_int_prop(int_prop.value());

		REQUIRE(test_struct.Int == 111);
	}

	SECTION("Nested Struct")
	{
		Test::NestedStruct nested;
		nested.Nested.Bool = true;
		PropertyList nested_props(nested);

		SECTION("Nested")
		{
			std::optional<Property> nested_bool = nested_props.Find("Bool");
			REQUIRE(!nested_bool);

			std::optional<Property> nested_int = nested_props.Find("Int");
			REQUIRE(!nested_int);

			nested_int = nested_props.Find("Nested.Int");
			REQUIRE(nested_int);

			test_int_prop(*nested_int);

			REQUIRE(nested.Nested.Int == 111);
		}
		
		SECTION("Offset")
		{
			std::optional<Property> second_int = nested_props.Find("SecondInt");
			REQUIRE(second_int);

			test_int_prop(*second_int);

			REQUIRE(nested.SecondInt == 111);
		}
	}
}

TEST_CASE("[Property] Redo")
{
	Test::SimpleStruct test_struct;

	PropertyList props(test_struct);
	std::optional<Property> int_prop = props.Find("Int");

	REQUIRE(int_prop->Get<int>() == 0);

	int_prop->Set(5);

	REQUIRE(int_prop->Get<int>() == 5);
	REQUIRE(int_prop->UndoSize() == 1);
	REQUIRE(int_prop->RedoSize() == 0);

	int_prop->Undo();

	REQUIRE(int_prop->Get<int>() == 0);
	REQUIRE(int_prop->UndoSize() == 0);
	REQUIRE(int_prop->RedoSize() == 1);

	int_prop->Redo();

	REQUIRE(int_prop->Get<int>() == 5);
	REQUIRE(int_prop->UndoSize() == 1);
	REQUIRE(int_prop->RedoSize() == 0);
}
