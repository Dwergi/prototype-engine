//
// Vector.h - A vector.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	template< typename T >
	class Vector
	{
	private:

		static const uint DefaultSize = 64;

	public:

		Vector()
			: m_data( nullptr ),
			m_entries( 0 ),
			m_capacity( 0 )
		{
			Resize( DefaultSize );
		}

		Vector( uint reserved_size )
			: m_data( nullptr ),
			m_entries( 0 ),
			m_capacity( 0 )
		{
			Resize( reserved_size );
		}

		T& operator[]( uint index )
		{
			return GetEntry( index );
		}

		const T& operator[]( uint index ) const
		{
			return GetEntry( index );
		}

		void Remove( uint index )
		{
			ASSERT( index < m_entries );

			// destruct the entry
			m_data[ index ].~T();

			--m_entries;
			
			if( index != m_entries )
				memcpy( &m_data[ index ], &m_data[ index ] + 1, (m_entries - index) * sizeof( T ) );
		}

		void Clear()
		{
			// call in-place destructors
			for( uint i = 0; i < m_entries; ++i )
			{
				m_data[ i ].~T();
			}

			m_entries = 0;
		}

		void Add( const T& value )
		{
			if( m_entries == m_capacity )
			{
				Resize( m_capacity * 2 );
			}

			m_data[ m_entries ] = value;
			++m_entries;
		}

		void Push( const T& value )
		{
			Add( value );
		}

		//
		// Insert at the given index. 
		// Index must be <= Size()
		//
		void Insert( const T& value, uint index )
		{
			ASSERT( index <= m_entries );

			if( index == m_entries )
			{
				Add( value );
			}
			else
			{
				memcpy( &m_data[ index ] + 1, &m_data[ index ], (m_entries - index) * sizeof( T ) );
				m_data[ index ] = value;
			}
		}

		T Pop()
		{
			ASSERT( m_entries > 0 );

			--m_entries;

			return m_data[ m_entries ];
		}

		//
		// Add all the entries from the given vector to the end of this vector.
		//
		void AddAll( const Vector<T>& other )
		{
			uint new_size = Size() + other.Size();

			uint new_capacity = m_capacity;
			while( new_size > new_capacity )
			{
				new_capacity *= 2;
			}

			if( new_capacity > m_capacity )
			{
				Resize( new_capacity );
			}
			
			for( uint i = 0; i < other.Size(); ++i )
			{
				m_data[ m_entries + i ] = other[ i ];
			}

			m_entries += other.Size();
		}

		T& Allocate()
		{
			if( m_entries == m_capacity )
			{
				Resize( m_capacity * 2 );
			}

			return m_data[ m_entries++ ];
		}

		void Allocate( T&& entry )
		{
			if( m_entries == m_capacity )
			{
				Resize( m_capacity * 2 );
			}

			m_data[ m_entries++ ] = entry;
		}

		void Swap( Vector<T>& other )
		{
			std::swap( m_data, other.m_data ); 
			std::swap( m_entries, other.m_entries );
			std::swap( m_capacity, other.m_capacity );
		}

		int IndexOf( const T& entry ) const
		{
			for( uint i = 0; i < m_entries; ++i )
			{
				if( m_data[ i ] == entry )
					return (int) i;
			}

			return -1;
		}

		bool Contains( const T& entry ) const
		{
			return IndexOf( entry ) != -1;
		}

		uint Size() const
		{
			return m_entries;
		}

		//
		// Iteration
		//
		class const_iterator
		{
		public:
			const T* Pointer;

			const_iterator() : Pointer( nullptr ) {}
			const_iterator( const T* ptr ) : Pointer( ptr ) {}

			inline const T& operator*() const { return *Pointer; }
			inline const_iterator& operator++() { ++Pointer; return *this; }
			inline const_iterator& operator+( size_t count ) { Pointer += count; return *this; }

			inline bool operator!=( const const_iterator& other ) const { return Pointer != other.Pointer; }
		};

		class iterator : public const_iterator
		{
		public:
			iterator() : const_iterator() {}
			iterator( T* ptr ) : const_iterator( ptr ) {}

			inline T& operator*() const { return *const_cast<T*>( Pointer ); }
		};

		inline const_iterator begin() const { return const_iterator( m_data ); }
		inline const_iterator end() const { return const_iterator( m_data + m_entries ); }

		inline iterator begin() { return iterator( m_data ); }
		inline iterator end() { return iterator( m_data + m_entries ); }

	private:

		void Resize( uint new_capacity )
		{
			ASSERT( new_capacity > m_capacity );

			T* new_data = reinterpret_cast<T*>( new char[ new_capacity * sizeof( T ) ] );

			if( m_data != nullptr )
			{
				memcpy( new_data, m_data, m_capacity * sizeof( T ) );
				delete[] m_data;
			}

			m_data = new_data;
			m_capacity = new_capacity;
		}

		T& GetEntry( uint index ) const
		{
			ASSERT( index < m_entries );

			return m_data[ index ];
		}

		uint m_entries;
		uint m_capacity;
		T* m_data;
	};
}