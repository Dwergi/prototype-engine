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
		Allocate(DefaultSize);
	}

	template <typename T>
	RingBuffer<T>::RingBuffer(uint64 size)
	{
		Allocate(size);
	}

	template <typename T>
	RingBuffer<T>::RingBuffer(const RingBuffer& other)
	{
		*this = other;
	}

	template <typename T>
	RingBuffer<T>::RingBuffer(RingBuffer&& other)
	{
		*this = std::move(other);
	}

	template <typename T>
	RingBuffer<T>& RingBuffer<T>::operator=(const RingBuffer& other)
	{
		for (uint64 count = 0; count < other.m_size; ++count)
		{
			uint64 index = (other.m_head + count) % other.Capacity();
			Push(other.m_storage[index]);
		}

		return *this;
	}

	template <typename T>
	RingBuffer<T>& RingBuffer<T>::operator=(RingBuffer&& other) noexcept
	{
		delete m_storage.Release();
		
		m_storage = other.m_storage.Release();
		m_head = other.m_head;
		m_tail = other.m_tail;
		m_size = other.m_size;
		return *this;
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
	void RingBuffer<T>::Push(const T& item)
	{
		uint64 size = Size();
		uint64 capacity = Capacity();
		if (size == (capacity - 1))
		{
			Grow();
		}

		new (&m_storage[m_tail]) T(item);

		m_tail = (m_tail + 1) % Capacity();
		++m_size;
	}

	template <typename T>
	void RingBuffer<T>::Push(T&& item)
	{
		uint64 size = Size();
		uint64 capacity = Capacity();
		if (size == (capacity - 1))
		{
			Grow();
		}

		new (&m_storage[m_tail]) T(std::move(item));

		m_tail = (m_tail + 1) % Capacity();
		++m_size;
	}

	template <typename T>
	T& RingBuffer<T>::Advance()
	{
		uint64 size = Size();
		uint64 capacity = Capacity();
		if (size == (capacity - 1))
		{
			Grow();
		}

		T& item = m_storage[m_tail];

		m_tail = (m_tail + 1) % Capacity();
		++m_size;

		return item;
	}

	template <typename T>
	T RingBuffer<T>::Pop()
	{
		DD_ASSERT(m_size != 0);

		uint64 old_head = m_head;

		m_head = (m_head + 1) % Capacity();

		--m_size;

		return m_storage[old_head];
	}

	template <typename T>
	uint64 RingBuffer<T>::Size() const
	{
		return m_size;
	}

	template <typename T>
	uint64 RingBuffer<T>::Capacity() const
	{
		return m_storage.Size();
	}

	template <typename T>
	void RingBuffer<T>::Grow()
	{
		uint64 old_size = m_storage.Size();
		uint64 new_size = old_size * 2;

		T* old_data = m_storage.Release();

		Allocate(new_size);

		if (m_tail < m_head)
		{
			uint64 front_count = old_size - m_head;
			CopyRange(old_data + m_head, m_storage.Access(), front_count);
			CopyRange(old_data, m_storage.Access() + front_count, m_tail);
		}
		else if (m_tail > m_head)
		{
			uint64 count = m_tail - m_head;
			CopyRange(old_data + m_head, m_storage.Access(), count);
		}

		m_head = 0;
		m_tail = m_size;
	}

	template <typename T>
	void RingBuffer<T>::Allocate(uint64 size)
	{
		DD_ASSERT(size > 0);

		m_storage.Set(reinterpret_cast<T*>(new char[size * sizeof(T)]), (size_t) size);
		memset(m_storage.Access(), 0xABAD1DEA, size * sizeof(T));
	}
}