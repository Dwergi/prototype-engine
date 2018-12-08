//
// RingBuffer.inl - A ring buffer.
// Copyright (C) Sebastian Nordgren 
// December 17th 2015
//

#include "ContainerHelpers.h"

namespace dd
{
	template <typename T>
	RingBuffer<T>::RingBuffer()
	{
		Allocate( DefaultSize );
	}

	template <typename T>
	RingBuffer<T>::RingBuffer( uint size )
	{
		Allocate( size );
	}

	template <typename T>
	RingBuffer<T>::~RingBuffer()
	{
		T* ptr = m_storage.Release();
		delete[] ptr;

		m_head = 0;
		m_tail = 0;
		m_size = 0;
	}

	template <typename T>
	void RingBuffer<T>::Push( const T& item )
	{
		uint size = Size();
		uint capacity = Capacity();
		if( size == (capacity - 1) )
		{
			Grow();
		}

		new (&m_storage[m_tail]) T( item );

		m_tail = (m_tail + 1) % Capacity();
		++m_size;
	}

	template <typename T>
	void RingBuffer<T>::Push( T&& item )
	{
		uint size = Size();
		uint capacity = Capacity();
		if( size == (capacity - 1) )
		{
			Grow();
		}

		new (&m_storage[m_tail]) T( std::move( item ) );

		m_tail = (m_tail + 1) % Capacity();
		++m_size;
	}

	template <typename T>
	T RingBuffer<T>::Pop()
	{
		DD_ASSERT( m_size != 0 );

		uint old_head = m_head;

		m_head = (m_head + 1) % Capacity();

		--m_size;

		return m_storage[old_head];
	}

	template <typename T>
	uint RingBuffer<T>::Size() const
	{
		return m_size;
	}

	template <typename T>
	uint RingBuffer<T>::Capacity() const
	{
		return m_storage.Size();
	}

	template <typename T>
	void RingBuffer<T>::Grow()
	{
		uint old_size = m_storage.Size();
		uint new_size = old_size * 2;

		T* old_data = m_storage.Release();

		Allocate( new_size );

		if( m_tail < m_head )
		{
			uint front_count = old_size - m_head;
			CopyRange( old_data + m_head, m_storage.Access(), front_count );
			CopyRange( old_data, m_storage.Access() + front_count, m_tail );
		}
		else if( m_tail > m_head )
		{
			uint count = m_tail - m_head;
			CopyRange( old_data + m_head, m_storage.Access(), count );
		}

		m_head = 0;
		m_tail = m_size;
	}

	template <typename T>
	void RingBuffer<T>::Allocate( uint size )
	{
		DD_ASSERT( size > 0 );

		m_storage.Set( reinterpret_cast<T*>(new char[size * sizeof( T )]), size );
		memset( m_storage.Access(), 0xABAD1DEA, size * sizeof( T ) );
	}
}