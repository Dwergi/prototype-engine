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
		memcpy( dest, src, sizeof( T ) );
	}

	template <typename T>
	inline void PlacementCopy( void* data, const void* src )
	{
		new (data) T( *(Cast<T>( src )) );
	}

	template <typename T>
	inline void PODPlacementCopy( void* data, const void* src )
	{
		memcpy( data, src, sizeof( T ) );
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
		memcpy( *dest, src, sizeof( T ) );
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

	template <typename T>
	inline void* ElementAt( void* container, uint index )
	{
		return &(*(T*) container)[index];
	}

	template <typename T>
	inline uint ContainerSize( void* container )
	{
		return ((T*) container)->Size();
	}

	template <typename TContainer, typename TItem>
	inline void InsertElement( void* container, uint index, void* elem )
	{
		return ((TContainer*) container)->Insert( *(TItem*) elem, index );
	}
}