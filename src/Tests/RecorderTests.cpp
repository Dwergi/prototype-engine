//
// RecorderTests.cpp - Tests for Recorder.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "Recorder.h"

int ITERATION_COUNT = 64;
int HISTORY_SIZE = 32;

TEST_CASE( "[Recorder] Recorder" )
{
	int ITERATION_COUNT = 64;
	int HISTORY_SIZE = 32;

	int value = -1;
	dd::Vector<int> values;
	dd::Recorder<int> test( value, HISTORY_SIZE );
	values.Add( test );

	for( int i = 0; i < ITERATION_COUNT; ++i )
	{
		test = i;
		values.Add( test );

		REQUIRE( value == test );
		REQUIRE( value == i );
	}

	int undo_count = test.GetUndoHistorySize();
	REQUIRE( undo_count == HISTORY_SIZE );

	for( int i = ITERATION_COUNT - 1; i >= ITERATION_COUNT - undo_count; --i )
	{
		test.Undo();

		REQUIRE( test == values[ i ] );
		REQUIRE( test.GetUndoHistorySize() >= 0 );
	}

	REQUIRE( test.GetUndoHistorySize() == 0 );
}

TEST_CASE( "[Recorder] FullRecorder" )
{
	int value = -1;
	std::vector<int> values;
	dd::FullRecorder<int> test( value, HISTORY_SIZE );
	values.push_back( test );

	for( int i = 0; i < ITERATION_COUNT; ++i )
	{
		test = i;
		values.push_back( test );

		REQUIRE( value == test );
		REQUIRE( value == i );
	}

	int undo_count = test.GetUndoHistorySize();
	REQUIRE( undo_count == HISTORY_SIZE );

	for( int i = ITERATION_COUNT - 1; i >= ITERATION_COUNT - undo_count; --i )
	{
		test.Undo();

		REQUIRE( test == values[ i ] );
		REQUIRE( test.GetUndoHistorySize() >= 0 );
	}

	REQUIRE( test.GetUndoHistorySize() == 0 );

	int redo_count = test.GetRedoHistorySize();
	REQUIRE( redo_count == HISTORY_SIZE );

	for( int i = ITERATION_COUNT - redo_count; i < ITERATION_COUNT; ++i )
	{
		test.Redo();

		REQUIRE( test == values[ i + 1 ] );
		REQUIRE( test.GetRedoHistorySize() >= 0 );
	}

	REQUIRE( test.GetRedoHistorySize() == 0 );
	REQUIRE( test.GetUndoHistorySize() == HISTORY_SIZE );
}