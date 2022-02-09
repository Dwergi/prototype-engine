//
// Vector.inl - Inline function definitions for dd::Vector
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "ContainerHelpers.h"

namespace dd
{
	template <typename T>
	const float Vector<T>::GrowthFactor = 1.7f;

	template <typename T>
	const uint64 Vector<T>::GrowthFudge = 8;

	template <typename T>
	const uint64 Vector<T>::DefaultSize = 8;

	template <typename T>
	Vector<T>::Vector()
		: m_data(nullptr)
	{

	}

	template <typename T>
	Vector<T>::Vector(T* data, uint64 size, uint64 capacity, bool can_delete) :
		m_data(data),
		m_size(size),
		m_capacity(capacity),
		m_deallocate(can_delete)
	{

	}

	template <typename T>
	Vector<T>::Vector(const Vector<T>& other)
		: m_data(nullptr)
	{
		Reallocate(other.m_capacity);
		CopyRange(other.m_data, m_data, other.m_size);
		m_size = other.m_size;
	}

	template <typename T>
	Vector<T>::Vector(Vector<T>&& other)
	{
		Swap(other);

		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}

	template <typename T>
	Vector<T>& Vector<T>::operator=(Vector&& other)
	{
		Swap(other);

		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;

		return *this;
	}

	template <typename T>
	Vector<T>& Vector<T>::operator=(const Vector& other)
	{
		Clear();
		Reallocate(other.m_capacity);
		CopyRange(other.m_data, m_data, other.m_size);
		
		m_size = other.m_size;

		return *this;
	}

	template <typename T>
	uint64 Vector<T>::Size() const
	{
		return m_size;
	}

	template <typename T>
	uint64 Vector<T>::Capacity() const
	{
		return m_capacity;
	}

	template <typename T>
	T& Vector<T>::operator[](uint64 index) const
	{
		return GetEntry(index);
	}

	template <typename T>
	T* Vector<T>::Data() const
	{
		return m_data;
	}

	template <typename T>
	void Vector<T>::Remove(uint64 index)
	{
		DD_ASSERT(index >= 0);
		DD_ASSERT(m_size > 0);
		DD_ASSERT(index < m_size);

		--m_size;

		// replace with the back
		new (&m_data[index]) T(m_data[m_size]);

		Zero(m_size);
	}

	template <typename T>
	void Vector<T>::RemoveItem(const T& item)
	{
		Vector<uint64> to_remove;

		for (uint64 i = 0; i < m_size; ++i)
		{
			if (m_data[i] == item)
			{
				to_remove.Add(i);
			}
		}

		// delete in reverse order so our indices don't get fucked
		to_remove.Reverse();

		for (uint64 i : to_remove)
		{
			Remove(i);
		}
	}

	template <typename T>
	void Vector<T>::RemoveOrdered(uint64 index)
	{
		DD_ASSERT(m_size > 0);
		DD_ASSERT(index < m_size);

		// destruct the entry
		Zero(index);

		--m_size;

		if (index != m_size)
		{
			MoveRange((&m_data[index]) + 1, &m_data[index], (m_size - index));
		}
	}

	template <typename T>
	void Vector<T>::RemoveAll(const Vector<T>& to_remove)
	{
		DD_ASSERT(m_size > 0);

		for (const T& entry : to_remove)
		{
			uint64 index = Find(entry);
			if (index >= 0)
			{
				Remove(index);
			}
		}
	}

	template <typename T>
	T Vector<T>::Pop()
	{
		DD_ASSERT(m_size > 0);

		--m_size;

		T entry = m_data[m_size];

		Zero(m_size);

		return entry;
	}

	template <typename T>
	void Vector<T>::Clear()
	{
		DestroyRange(m_data, m_size);

		memset(m_data, 0xFF, sizeof(T) * m_capacity);

		m_size = 0;
	}

	template <typename T>
	void Vector<T>::Zero(uint64 index) const
	{
		DD_ASSERT(index < m_capacity);

		m_data[index].~T();

		memset(&m_data[index], 0xFF, sizeof(T));
	}

	template <typename T>
	void Vector<T>::Add(T&& entry)
	{
		if (m_size == m_capacity)
		{
			Grow();
		}

		new (&m_data[m_size]) T(std::move(entry));

		++m_size;
	}

	template <typename T>
	void Vector<T>::Add(const T& entry)
	{
		if (m_size == m_capacity)
		{
			Grow();
		}

		new (&m_data[m_size]) T(entry);

		++m_size;
	}

	template <typename T>
	void Vector<T>::Insert(const T&& entry, uint64 index)
	{
		DD_ASSERT(index <= m_size);

		if (m_size == m_capacity)
		{
			Grow();
		}

		if (index == m_size)
		{
			Add(entry);
		}
		else
		{
			MoveRange(&m_data[index], &m_data[index + 1], m_size - index);

			new (&m_data[index]) T(entry);

			++m_size;
		}
	}

	template <typename T>
	void Vector<T>::Insert(const T& entry, uint64 index)
	{
		DD_ASSERT(index <= m_size);

		if (m_size == m_capacity)
		{
			Grow();
		}

		if (index == m_size)
		{
			Add(entry);
		}
		else
		{
			MoveRange(&m_data[index], &m_data[index + 1], m_size - index);

			new (&m_data[index]) T(entry);

			++m_size;
		}
	}

	template <typename T>
	void Vector<T>::AddAll(const Vector<T>& other)
	{
		uint64 new_size = Size() + other.Size();

		Grow(new_size);

		for (uint64 i = 0; i < other.Size(); ++i)
		{
			m_data[m_size + i] = other[i];
		}

		m_size += other.Size();
	}

	template <typename T>
	T& Vector<T>::Allocate()
	{
		if (m_size == m_capacity)
		{
			Grow();
		}

		T* ptr = new (&m_data[m_size]) T;

		++m_size;

		return *ptr;
	}

	template <typename T>
	T& Vector<T>::Allocate(T&& to_add)
	{
		if (m_size == m_capacity)
		{
			Grow();
		}

		T* ptr = new (&m_data[m_size]) T(std::move(to_add));

		++m_size;

		return *ptr;
	}

	template <typename T>
	void Vector<T>::Swap(Vector<T>& other)
	{
		std::swap(m_data, other.m_data);
		std::swap(m_size, other.m_size);
		std::swap(m_capacity, other.m_capacity);
	}

	template <typename T>
	uint64 Vector<T>::Find(const T& entry) const
	{
		for (uint64 i = 0; i < m_size; ++i)
		{
			if (m_data[i] == entry)
			{
				return i;
			}
		}

		return InvalidIndex;
	}

	template <typename T>
	void Vector<T>::Reverse()
	{
		for (uint64 i = 0; i < m_size / 2; ++i)
		{
			std::swap(m_data[i], m_data[m_size - i - 1]);
		}
	}

	template <typename T>
	bool Vector<T>::Contains(const T& entry) const
	{
		return Find(entry) != -1;
	}

	template <typename T>
	T& Vector<T>::Last() const
	{
		return m_data[m_size - 1];
	}

	template <typename T>
	T& Vector<T>::First() const
	{
		return m_data[0];
	}

	template <typename T>
	void Vector<T>::Resize(uint64 size)
	{
		if (size == m_capacity)
		{
			return;
		}

		if (size < m_capacity)
		{
			if (size < m_size)
			{
				DestroyRange(&m_data[size], m_size - size);
			}

			Reallocate(size);
		}
		else if (size > m_capacity)
		{
			Reallocate(size);

			ConstructRange(&m_data[m_size], size - m_size);
		}

		m_size = size;
	}

	template <typename T>
	void Vector<T>::Reserve(uint64 capacity)
	{
		if (capacity <= m_capacity)
		{
			return;
		}

		Reallocate(capacity);
	}

	template <typename T>
	void Vector<T>::ShrinkToFit()
	{
		Reallocate(m_size);
	}

	template <typename T>
	void Vector<T>::Grow()
	{
		// an unparametrized grow just grows by the default growth factor
		Reallocate((uint64) (m_capacity * GrowthFactor) + GrowthFudge);
	}

	template <typename T>
	void Vector<T>::Grow(uint64 target)
	{
		if (target <= m_capacity)
		{
			return;
		}

		uint64 new_capacity = m_capacity;
		while (new_capacity < target)
		{
			new_capacity = (uint64) (new_capacity * GrowthFactor) + GrowthFudge;
		}

		Reallocate(new_capacity);
	}

	template <typename T>
	void Vector<T>::Reallocate(uint64 new_capacity)
	{
		T* new_data = nullptr;

		if (new_capacity > 0)
		{
			new_data = reinterpret_cast<T*>(new char[new_capacity * sizeof(T)]);
			memset(new_data, 0xABAD1DEA, new_capacity * sizeof(T));
		}

		if (m_data != nullptr && new_data != nullptr)
		{
			if constexpr (std::is_move_constructible_v<T>)
			{
				MoveRange(m_data, new_data, ddm::min(m_size, new_capacity));
			}
			else if constexpr (std::is_copy_constructible_v<T>)
			{
				CopyRange(m_data, new_data, ddm::min(m_size, new_capacity));
				DestroyRange(m_data, m_size);
			}

			if (m_deallocate)
			{
				delete[]((char*) m_data);
			}
		}

		m_data = new_data;
		m_capacity = new_capacity;
	}

	template <typename T>
	T& Vector<T>::GetEntry(uint64 index) const
	{
		DD_ASSERT(index < m_capacity);

		return m_data[index];
	}

	template <typename T>
	bool Vector<T>::operator==(const Vector<T>& other) const
	{
		if (m_size != other.m_size)
			return false;

		for (uint64 i = 0; i < m_size; ++i)
		{
			if (m_data[i] != other.m_data[i])
				return false;
		}

		return true;
	}
}