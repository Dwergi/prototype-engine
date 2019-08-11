//
// DoubleBuffer.h 
// Copyright (C) Sebastian Nordgren 
// December 19th 2016
//

namespace dd
{
	

	template <typename T>
	DoubleBuffer<T>::DoubleBuffer( T* read, T* write, bool is_owner )
	{
		m_read = read;
		m_write = write;
		m_owner = is_owner;
	}

	template <typename T>
	DoubleBuffer<T>::~DoubleBuffer()
	{
		if (m_owner)
		{
			delete m_read;
			delete m_write;
		}
	}

	template <typename T>
	void DoubleBuffer<T>::Duplicate() const
	{
		Write() = Read();
	}

	template <typename T>
	void DoubleBuffer<T>::Swap()
	{
		std::swap(m_read, m_write);
	}

	template <typename T>
	const T& DoubleBuffer<T>::Read() const
	{
		return *reinterpret_cast<T*>( m_read );
	}

	template <typename T>
	T& DoubleBuffer<T>::Write() const
	{
		return *reinterpret_cast<T*>( m_write );
	}
}