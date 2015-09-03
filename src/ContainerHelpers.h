//
// ContainerHelpers.h - Various helpers for working with containers in a correct way.
// Copyright (C) Sebastian Nordgren 
// September 3rd 2015
//

template<typename T>
void CopyRange( const T* src, T* dest, uint count )
{
	for( uint i = 0; i < count; ++i )
	{
		new (&dest[ i ]) T( src[ i ] );
	}
}

template<typename T>
void ConstructRange( const T* src, uint count )
{
	for( uint i = 0; i < count; ++i )
	{
		new (&src[ i ]) T();
	}
}

template<typename T>
void DestroyRange( T* src, uint count )
{
	for( uint i = 0; i < count; ++i )
	{
		src[ i ].~T();
	}
}

template<typename T>
void MoveRange( const T* src, T* dest, uint count )
{
	ASSERT( count > 0 );
	ASSERT( src != nullptr && dest != nullptr );
	ASSERT( src != dest );

	for( uint i = 0; i < count; ++i )
	{
		new (&dest[i]) T( std::move( src[i] ) );
		src->~T();
	}
}