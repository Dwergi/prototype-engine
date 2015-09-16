//
// Array.h - An in-place, fixed-size array.
// Copyright (C) Sebastian Nordgren 
// September 3rd 2015
//

#pragma once

namespace dd
{
	template< typename T >
	class ArrayBase
	{
	public:
		uint Capacity() const { return m_capacity; }
		uint Size() const { return m_size; }

		virtual ~ArrayBase();
		ArrayBase<T>& operator=( const ArrayBase<T>& other );

		void Push( const T& value );
		void PushAll( const ArrayBase<T>& other );

		void Remove( uint index );
		void Remove( const T& value );

		T Pop();
		void Clear();

		const T& operator[]( uint index ) const;
		bool Contains( const T& val );
		int IndexOf( const T& val );		

		DEFINE_ITERATORS( T, m_data, m_size );

	protected:

		uint m_capacity;
		uint m_size;
		T* m_data;

		ArrayBase( T* buffer, uint capacity );
	};

	//
	// Array is, as the name implies, a static array that is allocated on the stack. Useful for temporary passing around of values. 
	//
	template< typename T, uint MaxCapacity >
	class Array 
		: public ArrayBase<T>
	{
	public:

		Array();
		Array( const Array<T, MaxCapacity>& other );

		virtual ~Array();

	private:

		T m_buffer[ MaxCapacity ];
	};

	//
	// Function definitions
	//

	template<typename T, uint MaxCapacity>
	Array<T, MaxCapacity>::Array()
		: ArrayBase( m_buffer, MaxCapacity )
	{
	}

	template<typename T, uint MaxCapacity>
	Array<T, MaxCapacity>::Array( const Array<T, MaxCapacity>& other )
		: ArrayBase( m_buffer, MaxCapacity )
	{
		CopyRange( other.m_buffer, m_buffer, other.m_size );
	}

	template<typename T, uint MaxCapacity>
	Array<T, MaxCapacity>::~Array()
	{

	}

	template <typename T>
	ArrayBase<T>::~ArrayBase()
	{
		m_data = nullptr;
		m_capacity = 0;
		m_size = 0;
	}

	template<typename T>
	ArrayBase<T>::ArrayBase( T* buffer, uint capacity )
		: m_data( buffer ),
		m_capacity( capacity ),
		m_size( 0 )
	{
	}

	template<typename T>
	ArrayBase<T>& ArrayBase<T>::operator=( const ArrayBase<T>& other )
	{
		ASSERT( other.Size() <= m_capacity );

		// clear, then push the entire other array
		Clear();
		PushAll( other );

		return *this;
	}

	template<typename T>
	const T& ArrayBase<T>::operator[]( uint index ) const
	{
		ASSERT( index < m_size, "Indexing unallocated memory!" );

		return m_data[ index ];
	}

	template<typename T>
	void ArrayBase<T>::Push( const T& value )
	{
		ASSERT( m_size < m_capacity );

		new (&m_data[ m_size ]) T( value );
		++m_size;
	}

	template<typename T>
	void ArrayBase<T>::PushAll( const ArrayBase<T>& other )
	{
		ASSERT( m_capacity - m_size >= other.Size() );

		for( uint i = 0; i < other.m_size; ++i )
		{
			Push( other[ i ] );
		}
	}

	template<typename T>
	T ArrayBase<T>::Pop()
	{
		ASSERT( m_size > 0 );

		--m_size;

		return m_data[ m_size ];
	}

	template<typename T>
	void ArrayBase<T>::Remove( uint index )
	{
		ASSERT( index < m_size );

		m_data[ index ].~T();

		MoveRange( &m_data[ index ] + 1, m_data[ index ], (m_size - index) - 1 );
	}

	template<typename T>
	void ArrayBase<T>::Remove( const T& value )
	{
		int index = IndexOf( value );
		if( index < 0 )
			return;

		Remove( (uint) index );
	}

	template<typename T>
	void ArrayBase<T>::Clear()
	{
		for( uint i = 0; i < m_size; ++i )
		{
			m_data[ i ].~T();
		}
		m_size = 0;
	}

	template<typename T>
	bool ArrayBase<T>::Contains( const T& val )
	{
		return IndexOf( val ) != -1;
	}

	template<typename T>
	int ArrayBase<T>::IndexOf( const T& val )
	{
		for( uint i = 0; i < m_size; ++i )
		{
			if( m_data[ i ] == val )
				return i;
		}

		return -1;
	}
}