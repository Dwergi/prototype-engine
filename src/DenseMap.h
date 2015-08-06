//
// DenseMap.h - A dense hash map.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

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
			if( m_entries == m_capacity )
			{
				Resize( m_capacity * 2 );
			}

			Insert( key, value );
		}

		void Remove( const TKey& key )
		{
			Entry& entry = GetEntry( key );
			Clear( entry );
		}

		TValue* operator[]( const TKey& key )
		{
			Entry& entry = GetEntry( key );

			if( IsEmpty( entry ) )
				return nullptr;

			return &entry.Value;
		}

		const TValue* operator[]( const TKey& key ) const
		{
			Entry& entry = GetEntry( key );

			if( IsEmpty( entry ) )
				return nullptr;

			return &entry.Value;
		}

	private:

		void Resize( uint new_size )
		{
			Entry* new_data = new Entry[ new_size ];
			memset( new_data, 0xFF, m_capacity * sizeof( Entry ) );

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
			memset( &entry, 0xFF, sizeof( Entry ) );
		}

		void Insert( const TKey& key, const TValue& value )
		{
			Entry& entry = GetEntry( key );
			if( IsEmpty( entry ) )
			{
				// slot is free
				entry.Key = key;
				entry.Value = value;
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