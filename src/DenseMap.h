//
// DenseMap.h - A dense hash map.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Exception.h"
#include "Hash.h"

namespace dd
{
	template< typename TKey, typename TValue >
	class DenseMap
	{
		static const uint DefaultSize = 16;

	public:

		struct Entry
		{
			TKey Key;
			TValue Value;
		};

		DenseMap();

		void SetHashFunction( void (*hash)( const TKey& ) );

		void Add( const TKey& key, const TValue& value );
		void Remove( const TKey& key );

		//
		// Find a key that may or may not exist. Returns null if it doesn't exist.
		//
		TValue* Find( const TKey& key ) const;
		bool Contains( const TKey& key ) const;
		TValue& operator[]( const TKey& key ) const;

		inline uint Size() const { return m_entries; }

		//
		// Iteration
		//
		class const_iterator
		{
		public:
			const DenseMap<TKey, TValue>* Map;
			const Entry* Pointer;

			const_iterator() : Pointer( nullptr ), Map( nullptr ) {}
			const_iterator( const Entry* ptr, const DenseMap<TKey, TValue>& map ) : Pointer( ptr ), Map( &map ) {}

			inline const Entry& operator*() const { return *Pointer; }
			inline const_iterator& operator++()
			{
				auto end = Map->end();

				do
				{
					++Pointer;
				} 
				while( Map->IsEmpty( *Pointer ) && *this != end );
				
				return *this;
			}

			inline bool operator!=( const const_iterator& other ) const { return Pointer != other.Pointer; }
		};

		class iterator : public const_iterator
		{
		public:
			iterator() : const_iterator() {}
			iterator( Entry& ptr ) : const_iterator( ptr ) {}

			inline Entry& operator*() const { return *const_cast<Entry&>( Pointer ); }
		};

		inline const_iterator begin() const { return const_iterator( m_data, *this ); }
		inline const_iterator end() const { return const_iterator( m_data + m_capacity, *this ); }

		inline iterator begin() { return iterator( m_data, *this ); }
		inline iterator end() { return iterator( m_data + m_capacity, *this ); }

	private:

		void Clear( Entry& entry ) const;
		void Insert( const TKey& key, const TValue& value );

		Entry* FindEntry( const TKey& key ) const;
		Entry& GetEntry( const TKey& key, uint* pIndex = nullptr ) const;
		bool IsMatch( const Entry& entry, const TKey& key ) const;
		bool IsEmpty( const Entry& entry ) const;

		void Grow();
		void Resize( uint new_size );
		void Rehash( const Entry* data, uint capacity );		

		uint m_entries;
		uint m_capacity;
		Entry* m_data;
		uint64 (*m_hash)( const TKey& );
	};

	template<typename TKey, typename TValue>
	DenseMap<TKey, TValue>::DenseMap()
		: m_entries( 0 ),
		m_capacity( 0 ),
		m_data( nullptr ),
		m_hash( &Hash )
	{
		Resize( DefaultSize );
	}

	template<typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::SetHashFunction( void (*hash)( const TKey& ) )
	{
		m_hash = hash;
	}

	template<typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Add( const TKey& key, const TValue& value )
	{
		ASSERT( m_capacity > 0 );

		if( m_entries == m_capacity )
		{
			Grow();
		}

		Insert( key, value );
	}

	template<typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Remove( const TKey& key )
	{
		ASSERT( m_entries > 0 );

		Entry* entry = FindEntry( key );
		if( entry == nullptr )
			throw dd::Exception( "Key not found!" );

		Clear( *entry );

		--m_entries;
	}

	template<typename TKey, typename TValue>
	bool DenseMap<TKey, TValue>::Contains( const TKey& key ) const
	{
		return Find( key ) != nullptr;
	}

	template<typename TKey, typename TValue>
	TValue* DenseMap<TKey, TValue>::Find( const TKey& key ) const
	{
		Entry* entry = FindEntry( key );

		if( entry == nullptr )
			return nullptr;

		// not found
		return &entry->Value;
	}

	template<typename TKey, typename TValue>
	typename DenseMap<TKey, TValue>::Entry* DenseMap<TKey, TValue>::FindEntry( const TKey& key ) const
	{
		uint index = 0;
		Entry& entry = GetEntry( key, &index );

		if( IsMatch( entry, key ) )
		{
			// simple case - no collision
			return &entry;
		}
		else
		{
			// hard case - collision, so linearly search for the next free slot
			Entry* current = &entry + 1;
			Entry* last = &m_data[ m_capacity - 1 ];

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

	template<typename TKey, typename TValue>
	TValue& DenseMap<TKey, TValue>::operator[]( const TKey& key ) const
	{
		return *Find( key );
	}

	template<typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Grow()
	{
		Resize( m_capacity * 2 );
	}

	template<typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Resize( uint new_size )
	{
		Entry* new_data = new Entry[ new_size ];
		memset( new_data, 0xFF, new_size * sizeof( Entry ) );

		Entry* old_data = m_data;
		uint old_capacity = m_capacity;

		m_capacity = new_size;
		m_data = new_data;

		if( old_data != nullptr )
		{
			Rehash( old_data, old_capacity );

			delete[] old_data;
		}
	}

	template<typename TKey, typename TValue>
	bool DenseMap<TKey, TValue>::IsMatch( const typename DenseMap<TKey, TValue>::Entry& entry, const TKey& key ) const
	{
		return !IsEmpty( entry ) && entry.Key == key;
	}

	template<typename TKey, typename TValue>
	bool DenseMap<TKey, TValue>::IsEmpty( const typename DenseMap<TKey, TValue>::Entry& entry ) const
	{
		const byte* bytes = reinterpret_cast<const byte*>( &entry.Key );
		uint length = sizeof( TKey );

		// check if all bytes of key are 0xFF
		for( uint i = 0; i < length; ++i )
		{
			if( bytes[ i ] != 0xFF ) 
				return false;
		}

		return true;
	}

	template<typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Rehash( const typename DenseMap<TKey, TValue>::Entry* data, uint capacity )
	{
		m_entries = 0; // clear this here, because we're going to be re-inserting everything

		for( uint i = 0; i < capacity; ++i ) 
		{
			if( !IsEmpty( data[ i ] ) )
			{
				Insert( data[ i ].Key, data[ i ].Value );
			}
		}
	}

	template<typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Clear( typename DenseMap<TKey, TValue>::Entry& entry ) const
	{
		entry.~Entry();

		memset( &entry, 0xFF, sizeof( Entry ) );
	}

	template<typename TKey, typename TValue>
	void DenseMap<TKey, TValue>::Insert( const TKey& key, const TValue& value )
	{
		uint index;
		Entry& entry = GetEntry( key, &index );

		if( IsEmpty( entry ) )
		{
			// slot is free, use placement new to construct it there
			new (&entry.Key) TKey( key );
			new (&entry.Value) TValue( value );

			++m_entries;
			return;
		}
		else
		{
			// find a new slot by linearly searching for the next free one
			Entry* current = &entry + 1;
			Entry* last = &m_data[ m_capacity - 1 ];

			while( current <= last )
			{
				if( IsEmpty( *current ) )
				{
					new (&current->Key) TKey( key );
					new (&current->Value) TValue( value );
					
					++m_entries;
					return;
				}

				++current;
			}
		}

		// couldn't find a slot, need to grow and try again
		Grow();
		Insert( key, value );
		
	}

	template<typename TKey, typename TValue>
	typename DenseMap<TKey, TValue>::Entry& DenseMap<TKey, TValue>::GetEntry( const TKey& key, uint* pIndex ) const
	{
		uint64 hash = m_hash( key );

		uint index = hash % m_capacity;

		if( pIndex != nullptr )
		{
			*pIndex = index;
		}

		return m_data[ index ];
	}
}