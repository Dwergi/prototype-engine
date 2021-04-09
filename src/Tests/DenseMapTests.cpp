#include "PCH.h"
#include "Tests.h"

#include "DenseMap.h"
#include "ColourComponent.h"

TEST_CASE( "[DenseMap] Add" )
{
	dd::DenseMap<int, int> map;

	map.Add( 0, 5 );
	map.Add( 5, 10 );

	REQUIRE( map.Contains( 0 ) );
	REQUIRE( map.Contains( 5 ) );
	REQUIRE( map[ 0 ] == 5 );
	REQUIRE( map[ 5 ] == 10 );
	REQUIRE( map.Size() == 2 );
}

struct Vector2Key
{
	int X;
	int Y;

	bool operator==( const Vector2Key& other ) const
	{
		return X == other.X
			&& Y == other.Y;
	}
};

TEST_CASE( "[DenseMap] Complex Keys" )
{
	dd::DenseMap<Vector2Key, int> map;

	Vector2Key keyA;
	keyA.X = 0;
	keyA.Y = 0;

	map.Add( keyA, 0 );

	REQUIRE( map.Contains( keyA ) );
	REQUIRE( map[ keyA ] == 0 );

	Vector2Key keyB;
	keyB.X = 5;
	keyB.Y = 5;

	map.Add( keyB, 10 );

	REQUIRE( map.Contains( keyB ) );
	REQUIRE( map[ keyB ] == 10 );

	dd::DenseMap<dd::String32 , int> string_map;
	dd::String32 test( "test string" );

	string_map.Add( test, 20 );
	
	REQUIRE( string_map.Contains( test ) );
	REQUIRE( string_map[ test ] == 20 );
}

TEST_CASE( "[DenseMap] Complex Values" )
{
	dd::DenseMap<int, Vector2Key> map;

	Vector2Key val;
	val.X = 20;
	val.Y = 20;

	map.Add( 0, val );

	REQUIRE( map.Contains( 0 ) );
	REQUIRE( map[ 0 ].X == 20 );
	REQUIRE( map[ 0 ].Y == 20 );
}

TEST_CASE( "[DenseMap] Remove" )
{
	dd::DenseMap<int, int> map;
	map.Add( 0, 10 );
	map.Add( 1, 20 );
	map.Add( 2, 30 );

	map.Remove( 0 );

	REQUIRE( !map.Contains( 0 ) );
	
	map.Remove( 1 );
	
	REQUIRE( !map.Contains( 1 ) );

	map.Remove(	2 );

	REQUIRE( !map.Contains( 2 ) );

	REQUIRE( map.Size() == 0 );
}

TEST_CASE( "[DenseMap] Resize" )
{
	dd::DenseMap<int, int> map;

	for( int i = 0; i < 128; ++i )
	{
		map.Add( i, i * 10 );
	}

	REQUIRE( map.Size() == 128 );

	for( int i = 0; i < 128; ++i )
	{
		REQUIRE( map[i] == i * 10 );
	}
}

TEST_CASE( "[DenseMap] MeshComponent Assign" )
{
	dd::DenseMap<int, dd::ColourComponent> a;
	dd::DenseMap<int, dd::ColourComponent> b;

	dd::ColourComponent colours[10];

	for( int i = 0; i < 10; ++i )
	{
		colours[i] = dd::ColourComponent();
		colours[i].Colour = glm::vec4( (float) i );
		a.Add( i, colours[i] );
	}

	b = a;

	for( int i = 0; i < 10; ++i )
	{
		REQUIRE( b[i].Colour == a[i].Colour );
		REQUIRE( b[i].Colour == colours[i].Colour );
	}
}