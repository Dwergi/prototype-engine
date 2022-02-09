//
// RecorderTests.cpp - Tests for Recorder.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#include "PCH.h"
#include "Tests.h"

#include "Property.h"

constexpr int ITERATION_COUNT = 64;

using namespace dd;

TEST_CASE("[UndoStack] Undo")
{
	int value = -1;
	Property prop(Variable(value), "Test");

	for (int i = 0; i < ITERATION_COUNT; ++i)
	{
		prop.Set(i);

		REQUIRE(value == prop.Get<int>());
		REQUIRE(value == i);

		REQUIRE(prop.UndoSize() == i + 1);
	}

	REQUIRE(prop.UndoSize() == ITERATION_COUNT);

	for (int i = ITERATION_COUNT - 1; i > 0; --i)
	{
		prop.Undo();

		REQUIRE(prop.Get<int>() == i - 1);
		REQUIRE(prop.UndoSize() >= 0);
	}

	prop.Undo();

	REQUIRE(prop.Get<int>() == -1);
	REQUIRE(prop.UndoSize() == 0);
}

TEST_CASE("[UndoStack] Redo")
{
	int value = -1;
	Property prop(Variable(value), "Test");

	for (int i = 0; i < ITERATION_COUNT; ++i)
	{
		prop.Set(i);

		REQUIRE(value == prop.Get<int>());
		REQUIRE(value == i);

		REQUIRE(prop.UndoSize() == i + 1);
	}

	REQUIRE(prop.UndoSize() == ITERATION_COUNT);

	for (int i = ITERATION_COUNT - 1; i > 0; --i)
	{
		prop.Undo();

		REQUIRE(prop.Get<int>() == i - 1);
		REQUIRE(prop.UndoSize() >= 0);
	}

	prop.Undo();
	REQUIRE(prop.Get<int>() == -1);
	REQUIRE(prop.UndoSize() == 0);
	REQUIRE(prop.RedoSize() == ITERATION_COUNT);

	for (int i = 0; i < ITERATION_COUNT; ++i)
	{
		prop.Redo();

		REQUIRE(prop.Get<int>() == i);
		REQUIRE(prop.RedoSize() == ITERATION_COUNT - (i + 1));
	}

	REQUIRE(prop.RedoSize() == 0);
	REQUIRE(prop.UndoSize() == ITERATION_COUNT);
}