//
// DoubleBufferTests.h - Tests for DoubleBuffer.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
#include "catch2/catch.hpp"

#include "DoubleBuffer.h"

using namespace dd;

TEST_CASE( "[DoubleBuffer] Swap" )
{
	Vector<int> v1, v2;
	DoubleBuffer<Vector<int>> buffer( &v1, &v2, false );

	{
		Vector<int>& write = buffer.Write();

		for( int i = 0; i < 16; ++i )
		{
			write.Add( i );
		}
	}

	buffer.Swap();

	{
		const Vector<int>& read = buffer.Read();
		for( int i = 0; i < 16; ++i )
		{
			REQUIRE( read[i] == i );
		}
	}
}

TEST_CASE( "[DoubleBuffer] Duplicate" )
{
	Vector<int> v1, v2;
	DoubleBuffer<Vector<int>> buffer( &v1, &v2, false );

	{
		Vector<int>& write = buffer.Write();

		for( int i = 0; i < 16; ++i )
		{
			write.Add( i );
		}
	}

	buffer.Swap();
	buffer.Duplicate();

	{
		Vector<int>& write = buffer.Write();
		for( int i = 0; i < 16; ++i )
		{
			REQUIRE( write[i] == i );
		}
	}
}

TEST_CASE( "[DoubleBuffer] Duplicate With Copy" )
{
	Vector<String16> v1, v2;
	
	DoubleBuffer<Vector<String16>> buffer( &v1, &v2, false );

	{
		Vector<String16>& write = buffer.Write();

		for( int i = 0; i < 16; ++i )
		{
			char buf[ 16 ] = { 0 };
			_itoa_s( i, buf, 10 );
			write.Add( String16( buf ) );
		}
	}

	buffer.Swap();
	buffer.Duplicate();

	{
		Vector<String16>& write = buffer.Write();
		for( int i = 0; i < 16; ++i )
		{
			int test = atoi( write[ i ].c_str() );
			REQUIRE( test == i );
		}
	}
}