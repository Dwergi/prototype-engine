//
// DoubleBuffer.h 
// Copyright (C) Sebastian Nordgren 
// December 19th 2016
//

namespace dd
{
	template <typename T>
	DoubleBuffer<T>::DoubleBuffer( T* read, T* write )
		: m_read( read ),
		m_write( write )
	{
	}

	template <typename T>
	DoubleBuffer<T>::~DoubleBuffer()
	{
	}

	template <typename T>
	void DoubleBuffer<T>::Swap()
	{
		std::swap( m_read, m_write );
	}

	template <typename T>
	void DoubleBuffer<T>::Clear()
	{
		m_write->Clear();
		m_read->Clear();
	}

	template <typename T>
	void DoubleBuffer<T>::Duplicate() const
	{
		*m_write = *m_read;
	}

	template <typename T>
	const T& DoubleBuffer<T>::GetRead() const
	{
		return *m_read;
	}

	template <typename T>
	T& DoubleBuffer<T>::GetWrite() const
	{
		return *m_write;
	}
}