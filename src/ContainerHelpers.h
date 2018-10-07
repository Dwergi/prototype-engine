//
// ContainerHelpers.h - Various helpers for working with containers in a correct way.
// Copyright (C) Sebastian Nordgren 
// September 3rd 2015
//

// A temporary buffer for copying containers around.
namespace dd
{
	const int BUFFER_SIZE = 2 * 1024 * 1024;
	static __declspec(thread) char s_buffer[BUFFER_SIZE];

	template <typename T>
	void CopyRange( const T* src, T* dest, uint count )
	{
		DD_ASSERT( (count * sizeof( T )) <= BUFFER_SIZE );

		T* temp = reinterpret_cast<T*>(s_buffer);

		for( uint i = 0; i < count; ++i )
		{
			new (&temp[i]) T( src[i] );
		}

		for( uint i = 0; i < count; ++i )
		{
			new (&dest[i]) T( std::move( temp[i] ) );
		}
	}

	template <typename T>
	void ConstructRange( T* src, uint count )
	{
		for( uint i = 0; i < count; ++i )
		{
			new (&src[i]) T();
		}
	}

	template <typename T>
	void DestroyRange( T* src, uint count )
	{
		for( uint i = 0; i < count; ++i )
		{
			src[i].~T();
		}
	}

	template <typename T>
	void MoveRange( const T* src, T* dest, uint count )
	{
		DD_ASSERT( src != nullptr && dest != nullptr );
		DD_ASSERT( src != dest );

		DD_ASSERT( (count * sizeof( T )) < BUFFER_SIZE );

		T* temp = reinterpret_cast<T*>(s_buffer);

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
}