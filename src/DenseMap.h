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
	private:

		struct Entry
		{
			TKey Key;
			TValue Value;
		};

		static const uint DefaultSize = 128;

	public:

		DenseMap()
			: m_entries( 0 ),
			m_capacity( 0 ),
			m_data( nullptr )
		{
			Resize( DefaultSize );
		}

		void Add( const TKey& key, const TValue& value )
		{
			ASSERT( m_capacity > 0 );

			if( m_entries == m_capacity )
			{
				Resize( m_capacity * 2 );
			}

			Insert( key, value );
		}

		void Remove( const TKey& key )
		{
			ASSERT( m_entries > 0 );

			Entry& entry = GetEntry( key );
			if( IsEmpty( entry ) )
				throw dd::Exception( "Key not found!" );

			Clear( entry );

			--m_entries;
		}

		bool Contains( const TKey& key ) const
		{
			Entry& entry = GetEntry( key );

			return !IsEmpty( entry );
		}

		TValue& operator[]( const TKey& key )
		{
			ASSERT( m_entries > 0 );

			Entry& entry = GetEntry( key );

			if( IsEmpty( entry ) )
				throw dd::Exception( "Key not found!" );

			return entry.Value;
		}

		const TValue& operator[]( const TKey& key ) const
		{
			ASSERT( m_entries > 0 );

			Entry& entry = GetEntry( key );

			if( IsEmpty( entry ) )
				throw dd::Exception( "Key not found!" );

			return entry.Value;
		}

		uint Size() const
		{
			return m_entries;
		}

	private:

		void Resize( uint new_size )
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

		bool IsEmpty( const Entry& entry ) const
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

		void Rehash( const Entry* data, uint capacity )
		{
			for( uint i = 0; i < capacity; ++i ) 
			{
				if( !IsEmpty( data[ i ] ) )
				{
					Insert( data[ i ].Key, data[ i ].Value );
				}
			}
		}

		void Clear( Entry& entry ) const
		{
			entry.~Entry();

			memset( &entry, 0xFF, sizeof( Entry ) );
		}

		void Insert( const TKey& key, const TValue& value )
		{
			Entry& entry = GetEntry( key );
			if( IsEmpty( entry ) )
			{
				// slot is free, use placement new to construct it there
				new( &entry.Key ) TKey( key );
				new( &entry.Value ) TValue( value );
			}
			else
			{
				// find a new slot by linearly searching for the next free one
				Entry* current = &entry + 1;

				while( current - m_data < m_capacity )
				{
					if( IsEmpty( *current ) )
					{
						current->Key = key;
						current->Value = value;
						return;
					}

					++current;
				}
			}

			++m_entries;
		}

		Entry& GetEntry( const TKey& key ) const
		{
			uint64 hash = Hash( key );

			uint index = hash % m_capacity;

			return m_data[ index ];
		}

		uint m_entries;
		uint m_capacity;
		Entry* m_data;
	};
}