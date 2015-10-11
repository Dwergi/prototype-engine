//
// TypeInfo.cpp - Run-time type information implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
// Significantly influenced by Randy Gaul (http://RandyGaul.net)
//

namespace dd
{
	template <typename T>
	struct EmptyType
	{
		// ...
	};

	template <typename T>
	inline T* Cast( void* data )
	{
		return reinterpret_cast<T*>(data);
	}

	template <typename T>
	inline const T* Cast( const void* data )
	{
		return reinterpret_cast<const T*>(data);
	}

	template <typename T>
	inline void* New()
	{
		T* data = (T*) malloc( sizeof( T ) );
		new (data) T;
		return data;
	}

	template <typename T>
	inline void* PODNew()
	{
		return malloc( sizeof( T ) );
	}

	template <typename T>
	inline void PlacementNew( void* data )
	{
		new (data) T;
	}

	template <typename T>
	inline void PODPlacementNew( void* data )
	{
	}

	template <typename T>
	inline void Copy( void* dest, const void* src )
	{
		*(Cast<T>( dest )) = *(Cast<T>( src ));
	}

	template <typename T>
	inline void PODCopy( void* dest, const void* src )
	{
		std::memcpy( dest, src, sizeof( T ) );
	}

	template <typename T>
	inline void PlacementCopy( void* data, const void* src )
	{
		new (data) T( *(Cast<T>( src )) );
	}

	template <typename T>
	inline void PODPlacementCopy( void* data, const void* src )
	{
		std::memcpy( data, src, sizeof( T ) );
	}

	template <typename T>
	inline void NewCopy( void** dest, const void* src )
	{
		T* newData = (T*) malloc( sizeof( T ) );
		new (newData) T( *Cast<T>( src ) );
		*dest = newData;
	}

	template <typename T>
	inline void PODNewCopy( void** dest, const void* src )
	{
		*dest = malloc( sizeof( T ) );
		std::memcpy( *dest, src, sizeof( T ) );
	}

	template <typename T>
	inline void Delete( void* data )
	{
		Cast<T>( data )->~T();
		free( data );
	}

	template <typename T>
	inline void PODDelete( void* data )
	{
		free( data );
	}

	template <typename T>
	inline void PlacementDelete( void* data )
	{
		Cast<T>( data )->~T();
	}

	template <typename T>
	inline void PODPlacementDelete( void* data )
	{
	}

	template <typename T>
	struct HasDefaultCtor
	{
		template <typename U>
		static int SFINAE( decltype( U() )* );
		template <typename U>
		static uint8 SFINAE( ... );

		static const bool value = sizeof( SFINAE<T>( nullptr ) ) == sizeof( int );
	};

	template <typename T>
	struct HasCopyCtor
	{
		static T MakeT();

		template <typename U>
		static int SFINAE( decltype( U( MakeT() ) )* );
		template <typename U>
		static uint8 SFINAE( ... );

		static const bool value = sizeof( SFINAE<T>( nullptr ) ) == sizeof( int );
	};

	template <bool B, typename T = void>
	struct disable_if {
		typedef T type;
	};

	template <typename T>
	struct disable_if<true,T> {
	};

	template <bool, class T, T v>
	struct SetFunc
	{
		static T Get()
		{
			return v;
		};
	};

	template <class T, T v>
	struct SetFunc<false, T, v>
	{
		static T Get()
		{
			return nullptr;
		};
	};
}