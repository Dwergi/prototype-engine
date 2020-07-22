//
// Array.inl - Inline functions for dd::Array.
// Copyright (C) Sebastian Nordgren 
// September 3rd 2015
//

template <typename T, int MaxCapacity>
Array<T, MaxCapacity>::Array()
	: IArray(m_inline, MaxCapacity)
{
}

template <typename T, int MaxCapacity>
Array<T, MaxCapacity>::Array(const Array<T, MaxCapacity>& other)
	: IArray(m_inline, MaxCapacity)
{
	m_size = other.m_size;

	CopyRange(other.m_inline, m_inline, m_size);
}

template <typename T, int MaxCapacity>
Array<T, MaxCapacity>::Array(Array<T, MaxCapacity>&& other) noexcept
	: IArray(m_inline, MaxCapacity)
{
	m_size = other.m_size;

	MoveRange(other.m_inline, m_inline, m_size);
}

template <typename T, int MaxCapacity>
Array<T, MaxCapacity>::~Array()
{

}

template <typename T, int MaxCapacity>
Array<T, MaxCapacity>& Array<T, MaxCapacity>::operator=(const Array<T, MaxCapacity>& other)
{
	Clear();
	PushAll(other);

	return *this;
}
	
template <typename T, int MaxCapacity>
Array<T, MaxCapacity>& Array<T, MaxCapacity>::operator=(Array<T, MaxCapacity>&& other)
{
	Clear();
	PushAll(std::move(other));

	return *this;
}

template <typename T>
IArray<T>::~IArray()
{
	m_data.Release();
	m_size = 0;
}

template <typename T>
IArray<T>::IArray(T* buffer, int capacity) :
	m_data(buffer, capacity),
	m_size(0)
{
	std::memset(m_data.Access(), 0, m_data.SizeBytes());
}

template <typename T>
IArray<T>& IArray<T>::operator=(const IArray<T>& other)
{
	DD_ASSERT(other.Size() <= m_data.Size());

	// clear, then push the entire other array
	Clear();
	PushAll(other);

	return *this;
}

template <typename T>
IArray<T>& IArray<T>::operator=(IArray<T>&& other)
{
	DD_ASSERT(other.Size() <= m_data.Size());

	// clear, then push the entire other array
	Clear();
	PushAll(std::move(other));

	return *this;
}

template <typename T>
const T& IArray<T>::operator[](size_t index) const
{
	DD_ASSERT(index < m_size, "Indexing unallocated memory!");

	return m_data[index];
}

template <typename T>
T& IArray<T>::Push(const T& value)
{
	DD_ASSERT(m_size < m_data.Size());

	T* entry = new (&m_data[m_size]) T(value);
	++m_size;

	return *entry;
}

template <typename T>
T& IArray<T>::Push(T&& value)
{
	DD_ASSERT(m_size < m_data.Size());

	T* entry = new (&m_data[m_size]) T(std::move(value));
	++m_size;
		
	return *entry;
}

template <typename T>
void IArray<T>::PushAll(const IArray<T>& other)
{
	DD_ASSERT(m_data.Size() - m_size >= other.Size());

	for (int i = 0; i < other.m_size; ++i)
	{
		Push(other[i]);
	}
}

template <typename T>
void IArray<T>::PushAll(IArray<T>&& other)
{
	DD_ASSERT(m_data.Size() - m_size >= other.Size());

	for (int i = 0; i < other.m_size; ++i)
	{
		Push(std::move(other[i]));
	}
}

template <typename T>
T IArray<T>::Pop()
{
	DD_ASSERT(m_size > 0);

	--m_size;

	T value = m_data[m_size];

	m_data[m_size].~T();

	return value;
}

template <typename T>
void IArray<T>::Fill(const T& value)
{
	m_data.Fill(value);
}

template <typename T>
void IArray<T>::RemoveAt(int index)
{
	DD_ASSERT(index < m_size);

	m_data[index].~T();

	MoveRange(m_data.Access() + index + 1, m_data.Access() + index, (m_size - index) - 1);

	--m_size;
}

template <typename T>
void IArray<T>::Remove(const T& value)
{
	int index = IndexOf(value);
	if (index < 0)
		return;

	RemoveAt(( uint) index);
}

template <typename T>
void IArray<T>::Clear()
{
	for (int i = 0; i < m_size; ++i)
	{
		m_data[i].~T();
	}
	m_size = 0;
}

template <typename T>
bool IArray<T>::Contains(const T& val)
{
	return IndexOf(val) != -1;
}

template <typename T>
int IArray<T>::IndexOf(const T& val)
{
	for (int i = 0; i < m_size; ++i)
	{
		if (m_data[i] == val)
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
