//
// DenseMap.inl - A dense hash map.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

namespace dd
{
	//
	// Iterator
	//
	template <typename TKey, typename TValue>
	class DenseMapIterator
	{
	private:

		typedef typename DenseMap<TKey, TValue> TMap;
		typedef typename DenseMap<TKey, TValue>::Entry TMapEntry;

	public:
		const TMap* Map;
		const TMapEntry* Pointer;

		DenseMapIterator()
			: Pointer( nullptr ), Map( nullptr ) {}

		DenseMapIterator( const TMapEntry* ptr, const TMap& map )
			: Pointer( ptr ), 
			Map( &map ) {}

		inline TMapEntry& operator*() { return const_cast<TMapEntry&>( *Pointer ); }
		inline const TMapEntry& operator*() const { return *Pointer; }

		inline DenseMapIterator<TKey, TValue>& operator++()
		{
			auto end = Map->end();
			do
			{
				++Pointer;
			} while( Map->IsEmpty( *Pointer ) && *this != end );

			return *this;
		}

		inline bool operator!=( const DenseMapIterator<TKey, TValue>& other ) const
		{
			return Pointer != other.Pointer;
		}
	};

	template <typename TKey, typename TValue>
	DenseMap<TKey, TValue>::DenseMap() :
		m_hash( &Hash )
	{
		Resize( DefaultSize );
	}

	template <typename TKey, typename TValue>
	DenseMap<TKey, TValue>::DenseMap( DenseMap&& other ) :
		m_data( std::move( other.m_data ) ),
		m_hash( std::move( other.m_hash ) )
	{

	}

	template <typename TKey, typename TValue>
	DenseMap<TKey, TValue>::DenseMap( const DenseMap& other ) :
		m_hash( other.m_hash )
	{
		Resize( other.m_data.Size() );
		Rehash( other.m_data );
	}

	template <typename TKey, typename TValue>
	DenseMap<TKey, TValue>::~DenseMap()
	{
		Clear();

		delete[] (byte*) m_data.Release();
	}

	template <typename TKey, typename TValue>
	DenseMap<TKey, TValue>& DenseMap<TKey, TValue>::operator=( DenseMap<TKey, TValue>&& other )
	{
		m_hash = other.m_hash;
		m_data = std::move( other.m_data );
		m_entries = other.m_entries;

		return *this;
	}

	template <typename TKey, typename TValue>
	DenseMap<TKey, TValue>& DenseMap<TKey, TValue>::operator=( const DenseMap<TKey, TValue>& other )
	{
		Clear();
		Resize( other.m_data.Size() );

		m_hash = other.m_hash;
		Rehash( other.m_data );

		return *this;
	}

	template <typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::SetHashFunction( void( *hash )(const TKey&) )
	{
		m_hash = hash;
	}

	template <typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Add( const TKey& key, const TValue& value )
	{
		DD_ASSERT( m_data.Size() > 0 );

		if( m_entries == m_data.Size() )
		{
			Grow();
		}

		Insert( key, value );
	}

	template <typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Remove( const TKey& key )
	{
		DD_ASSERT( m_entries > 0 );

		Entry* entry = FindEntry( key );
		if( entry == nullptr )
			throw dd::Exception( "Key not found!" );

		Clear( *entry );

		--m_entries;
	}

	template <typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Clear()
	{
		for( uint i = 0; i < m_data.Size(); ++i )
		{
			if( !IsEmpty( m_data[i] ) )
			{
				Clear( m_data[i] );
			}
		}

		m_entries = 0;
	}

	template <typename TKey, typename TValue>
	bool DenseMap<TKey, TValue>::Contains( const TKey& key ) const
	{
		return Find( key ) != nullptr;
	}

	template <typename TKey, typename TValue>
	TValue* DenseMap<TKey, TValue>::Find( const TKey& key ) const
	{
		Entry* entry = FindEntry( key );

		if( entry == nullptr )
			return nullptr;

		// not found
		return &entry->Value;
	}

	template <typename TKey, typename TValue>
	typename DenseMap<TKey, TValue>::Entry* DenseMap<TKey, TValue>::FindEntry( const TKey& key ) const
	{
		uint index = 0;
		Entry& entry = GetEntry( key );

		if( IsMatch( entry, key ) )
		{
			// simple case - no collision
			return &entry;
		}
		else
		{
			// hard case - collision, so linearly search for the next free slot
			Entry* current = &entry + 1;
			Entry* last = &m_data[m_data.Size() - 1];

			while( current <= last )
			{
				if( IsMatch( *current, key ) )
				{
					return current;
				}

				++current;
			}
		}

		// not found
		return nullptr;
	}

	template <typename TKey, typename TValue>
	TValue& DenseMap<TKey, TValue>::operator[]( const TKey& key ) const
	{
		return *Find( key );
	}

	template <typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Grow()
	{
		Resize( m_data.Size() * 2 );
	}

	template <typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Reserve( uint size )
	{
		Resize( size );
	}

	template <typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Resize( uint new_capacity )
	{
		Entry* new_data = reinterpret_cast<Entry*>( new byte[new_capacity * sizeof( Entry )] );
		memset( new_data, 0xFF, new_capacity * sizeof( Entry ) );

		Buffer<Entry> old_data( m_data );
		m_data.Release();
		m_data.Set( new_data, new_capacity );

		if( old_data.Size() > 0 )
		{
			Rehash( old_data );

			for( uint i = 0; i < old_data.Size(); ++i )
			{
				if( !IsEmpty( old_data[i] ) )
				{
					Clear( old_data[i] );
				}
			}

			delete[] (byte*) old_data.Release();
		}
	}

	template <typename TKey, typename TValue>
	bool DenseMap<TKey, TValue>::IsMatch( const typename DenseMap<TKey, TValue>::Entry& entry, const TKey& key ) const
	{
		return !IsEmpty( entry ) && entry.Key == key;
	}

	template <typename TKey, typename TValue>
	bool DenseMap<TKey, TValue>::IsEmpty( const typename DenseMap<TKey, TValue>::Entry& entry ) const
	{
		const byte* bytes = reinterpret_cast<const byte*>(&entry.Key);
		uint length = sizeof( TKey );

		// check if all bytes of key are 0xFF
		for( uint i = 0; i < length; ++i )
		{
			if( bytes[i] != 0xFF )
				return false;
		}

		return true;
	}

	template <typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Rehash( const Buffer<typename DenseMap<TKey, TValue>::Entry>& from )
	{
		m_entries = 0; // clear this here, because we're going to be re-inserting everything

		for( uint i = 0; i < from.Size(); ++i )
		{
			if( !IsEmpty( from[i] ) )
			{
				Insert( from[i].Key, from[i].Value );
			}
		}
	}

	template <typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Clear( typename DenseMap<TKey, TValue>::Entry& entry ) const
	{
		entry.~Entry();

		memset( &entry, 0xFF, sizeof( Entry ) );
	}

	template <typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Insert( const TKey& key, const TValue& value )
	{
		Entry& entry = GetEntry( key );

		if( IsEmpty( entry ) )
		{
			// slot is free, create entry
			CreateEntry( &entry, key, value );
			return;
		}
		else
		{
			// find a new slot by linearly searching for the next free one
			Entry* current = &entry + 1;
			Entry* last = &m_data[m_data.Size() - 1];

			while( current <= last )
			{
				if( IsEmpty( *current ) )
				{
					CreateEntry( current, key, value );
					return;
				}

				++current;
			}
		}

		// couldn't find a slot, need to grow and try again
		Grow();
		Insert( key, value );
	}

	template <typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::CreateEntry( Entry* ptr, const TKey& key, const TValue& value )
	{
		new (&ptr->Key) TKey( key );
		new (&ptr->Value) TValue( value );

		++m_entries;
	}

	template <typename TKey, typename TValue>
	typename DenseMap<TKey, TValue>::Entry& DenseMap<TKey, TValue>::GetEntry( const TKey& key ) const
	{
		uint64 hash = m_hash( key );

		uint index = hash % m_data.Size();
		
		return m_data[index];
	}
}