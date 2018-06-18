//
// Array.inl - Inline functions for dd::Array.
// Copyright (C) Sebastian Nordgren 
// September 3rd 2015
//

namespace dd
{
	template <typename T, uint MaxCapacity>
	Array<T, MaxCapacity>::Array()
		: IArray( m_buffer, MaxCapacity )
	{
	}

	template <typename T, uint MaxCapacity>
	Array<T, MaxCapacity>::Array( const Array<T, MaxCapacity>& other )
		: IArray( m_buffer, MaxCapacity )
	{
		CopyRange( other.m_buffer, m_buffer, other.m_size );

		m_size = other.m_size;
	}

	template <typename T, uint MaxCapacity>
	Array<T, MaxCapacity>::~Array()
	{

	}

	template <typename T>
	IArray<T>::~IArray()
	{
		m_data = nullptr;
		m_capacity = 0;
		m_size = 0;
	}

	template <typename T>
	IArray<T>::IArray( T* buffer, uint capacity )
		: m_data( buffer ),
		m_capacity( capacity ),
		m_size( 0 )
	{
	}

	template <typename T>
	IArray<T>& IArray<T>::operator=( const IArray<T>& other )
	{
		DD_ASSERT( other.Size() <= m_capacity );

		// clear, then push the entire other array
		Clear();
		PushAll( other );

		return *this;
	}

	template <typename T>
	const T& IArray<T>::operator[]( size_t index ) const
	{
		DD_ASSERT( index < m_size, "Indexing unallocated memory!" );

		return m_data[index];
	}

	template <typename T>
	void IArray<T>::Push( const T& value )
	{
		DD_ASSERT( m_size < m_capacity );

		new (&m_data[m_size]) T( value );
		++m_size;
	}

	template <typename T>
	void IArray<T>::Add( const T& value )
	{
		Push( value );
	}

	template <typename T>
	void IArray<T>::PushAll( const IArray<T>& other )
	{
		DD_ASSERT( m_capacity - m_size >= other.Size() );

		for( uint i = 0; i < other.m_size; ++i )
		{
			Push( other[i] );
		}
	}

	template <typename T>
	T IArray<T>::Pop()
	{
		DD_ASSERT( m_size > 0 );

		--m_size;

		T value = m_data[m_size];

		m_data[ m_size ].~T();

		return value;
	}

	template <typename T>
	void IArray<T>::RemoveAt( uint index )
	{
		DD_ASSERT( index < m_size );

		m_data[index].~T();

		MoveRange( m_data + index + 1, m_data + index, (m_size - index) - 1 );

		--m_size;
	}

	template <typename T>
	void IArray<T>::Remove( const T& value )
	{
		int index = IndexOf( value );
		if( index < 0 )
			return;

		RemoveAt( (uint) index );
	}

	template <typename T>
	void IArray<T>::Clear()
	{
		for( uint i = 0; i < m_size; ++i )
		{
			m_data[i].~T();
		}
		m_size = 0;
	}

	template <typename T>
	bool IArray<T>::Contains( const T& val )
	{
		return IndexOf( val ) != -1;
	}

	template <typename T>
	int IArray<T>::IndexOf( const T& val )
	{
		for( uint i = 0; i < m_size; ++i )
		{
			if( m_data[i] == val )
				return i;
		}

		return -1;
	}

	template <typename T>
	T& IArray<T>::Last() const
	{
		return m_data[m_size - 1];
	}

	template <typename T>
	T& IArray<T>::First() const
	{
		return m_data[0];
	}
}