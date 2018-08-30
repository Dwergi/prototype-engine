//
// DoubleBuffer.h 
// Copyright (C) Sebastian Nordgren 
// December 19th 2016
//

namespace dd
{
	

	template <typename T>
	DoubleBuffer<T>::DoubleBuffer( T* read, T* write, bool is_owner )
		: IDoubleBuffer( read, write, is_owner )
	{
	}

	template <typename T>
	DoubleBuffer<T>::~DoubleBuffer()
	{
	}

	template <typename T>
	void DoubleBuffer<T>::Duplicate() const
	{
		Write() = Read();
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