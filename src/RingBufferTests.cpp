//
// RingBufferTests.cpp - Tests for RingBuffer.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PrecompiledHeader.h"

#include "catch2/catch.hpp"

#include "RingBuffer.h"

TEST_CASE( "[RingBuffer] Push" )
{
	dd::RingBuffer<int> buffer;

	buffer.Push( 0 );
	buffer.Push( 1 );
	buffer.Push( 2 );
	buffer.Push( 3 );

	REQUIRE( buffer.Size() == 4 );

	int res = buffer.Pop();
	REQUIRE( res == 0 );
	REQUIRE( buffer.Size() == 3 );

	res = buffer.Pop();
	REQUIRE( res == 1 );

	res = buffer.Pop();
	REQUIRE( res == 2 );

	res = buffer.Pop();
	REQUIRE( res == 3 );

	REQUIRE( buffer.Size() == 0 );
}

TEST_CASE( "[RingBuffer] Cycle" )
{
	dd::RingBuffer<int> buffer;

	for( int i = 0; i < 16; ++i )
	{
		buffer.Push( i );
		int res = buffer.Pop();

		REQUIRE( buffer.Size() == 0 );
		REQUIRE( res == i );
	}
}

TEST_CASE( "[RingBuffer] Grow" )
{
	dd::RingBuffer<int> buffer;

	for( int i = 0; i < 16; ++i )
	{
		buffer.Push( i );
		REQUIRE( buffer.Size() == i + 1 );
	}

	for( int i = 0; i < 16; ++i )
	{
		int res = buffer.Pop();
		REQUIRE( res == i );
	}
}

TEST_CASE( "[RingBuffer] Grow Non-Zero" )
{
	dd::RingBuffer<int> buffer;

	// move the head and tail forward a bit
	for( int i = 0; i < 4; ++i )
	{
		buffer.Push( i );
		int res = buffer.Pop();
	}

	for( int i = 0; i < 16; ++i )
	{
		buffer.Push( i );
		REQUIRE( buffer.Size() == i + 1 );
	}

	for( int i = 0; i < 16; ++i )
	{
		int res = buffer.Pop();
		REQUIRE( res == i );
	}
}
