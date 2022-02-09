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
	template <typename TKey, typename TValue>
	class DenseMapIterator;

	template <typename TKey, typename TValue>
	class DenseMap
	{
		static const uint64 DefaultSize = 16;
		const float MaxOccupancy = 0.75f;
		static const uint64 OverflowSize = 16;

	public:

		struct Entry
		{
			bool Used;
			TKey Key;
			TValue Value;
		};

		DenseMap();
		DenseMap( DenseMap<TKey, TValue>&& other );
		DenseMap( const DenseMap<TKey, TValue>& other );
		~DenseMap();

		DenseMap<TKey, TValue>& operator=( DenseMap<TKey, TValue>&& other );
		DenseMap<TKey, TValue>& operator=( const DenseMap<TKey, TValue>& other );

		void Reserve( int size );

		void SetHashFunction( void (*hash)( const TKey& ) );

		void Add( const TKey& key, const TValue& value );
		void Remove( const TKey& key );
		void Clear();

		//
		// Find a key that may or may not exist. Returns null if it doesn't exist.
		//
		TValue* Find( const TKey& key ) const;
		bool Contains( const TKey& key ) const;
		TValue& operator[]( const TKey& key ) const;

		inline int Size() const { return m_entries; }

		DenseMapIterator<TKey, TValue> begin() const;
		inline DenseMapIterator<TKey, TValue> end() const { return DenseMapIterator<TKey, TValue>( m_data.Get() + m_data.Size(), *this ); }
		
	private:

		friend class DenseMapIterator<TKey, TValue>;
		static const uint64 EntrySize = sizeof( Entry );

		int m_entries { 0 };
		Buffer<Entry> m_data;
		Buffer<Entry> m_overflow;

		uint64 (*m_hash)( const TKey& );

		void Clear( Entry& entry ) const;
		void Insert( const TKey& key, const TValue& value );

		Entry* FindEntry( const TKey& key ) const;
		Entry& GetEntry( const TKey& key ) const;
		void CreateEntry( Entry* ptr, const TKey& key, const TValue& value );

		bool IsMatch( const Entry& entry, const TKey& key ) const;
		bool IsEmpty( const Entry& entry ) const;

		void Grow();
		void Resize( uint64 new_size );
		void Rehash( const Buffer<Entry>& data );
	};
}

#include "DenseMap.inl"