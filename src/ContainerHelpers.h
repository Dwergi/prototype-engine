//
// ContainerHelpers.h - Various helpers for working with containers in a correct way.
// Copyright (C) Sebastian Nordgren 
// September 3rd 2015
//

const int MAX_COUNT = 2048;

template<typename T>
void CopyRange( const T* src, T* dest, uint count )
{
	ASSERT( count <= MAX_COUNT );

	static __declspec(thread) char buffer[sizeof(T) * MAX_COUNT];
	T* temp = (T*) buffer;

	for( uint i = 0; i < count; ++i )
	{
		new (&temp[i]) T( src[i] );
	}

	for( uint i = 0; i < count; ++i )
	{
		new (&dest[i]) T( std::move( temp[i] ) );
	}
}

template<typename T>
void ConstructRange( T* src, uint count )
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
		src[i].~T();
	}
}

template<typename T>
void MoveRange( const T* src, T* dest, uint count )
{
	ASSERT( count > 0 );
	ASSERT( src != nullptr && dest != nullptr );
	ASSERT( src != dest );

	ASSERT( count <= MAX_COUNT );

	static __declspec(thread) char buffer[sizeof( T ) * MAX_COUNT];
	T* temp = (T*) buffer;

	for( uint i = 0; i < count; ++i )
	{
		new (&temp[i]) T( std::move( src[i] ) );
		src[i].~T();
	}

	for( uint i = 0; i < count; ++i )
	{
		new (&dest[i]) T( std::move( temp[i] ) );
	}
}
