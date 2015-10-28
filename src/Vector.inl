//
// Vector.inl - Inline function definitions for dd::Vector
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

namespace dd
{
	template<typename T>
	Vector<T>::Vector()
		: m_data( nullptr )
	{
		
	}

	template<typename T>
	Vector<T>::Vector( uint reserved )
		: m_data( nullptr )
	{
		Reserve( reserved );
	}

	template<typename T>
	Vector<T>::Vector( const Vector<T>& other )
		: m_data( nullptr )
	{
		Reallocate( other.m_capacity );
		CopyRange( other.m_data, m_data, other.m_size );
		m_size = other.m_size;
	}

	template<typename T>
	Vector<T>::Vector( Vector<T>&& other )
	{
		Swap( other );

		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}

	template<typename T>
	Vector<T>& Vector<T>::operator=( const Vector& other )
	{
		Clear();
		Reallocate( other.m_capacity );
		CopyRange( other.m_data, m_data, other.m_size );
		m_size = other.m_size;

		return *this;
	}

	template<typename T>
	T& Vector<T>::operator[]( uint index ) const
	{
		return GetEntry( index );
	}

	template<typename T>
	void Vector<T>::Remove( uint index )
	{
		ASSERT( m_size > 0 );
		ASSERT( index < m_size );

		--m_size;

		// replace with the back
		new (&m_data[index]) T( m_data[m_size] );

		Zero( m_size );
	}

	template<typename T>
	void Vector<T>::Remove( int index )
	{
		if( index < 0 )
			return;

		Remove( (uint) index );
	}

	template<typename T>
	void Vector<T>::RemoveItem( const T& item )
	{
		int index = -1;
		for( uint i = 0; i < m_size; ++i )
		{
			if( m_data[i] == item )
			{
				index = i;
				break;
			}
		}

		Remove( index );
	}

	template<typename T>
	void Vector<T>::RemoveOrdered( uint index )
	{
		ASSERT( m_size > 0 );
		ASSERT( index < m_size );

		// destruct the entry
		Zero( index );

		--m_size;

		if( index != m_size )
		{
			MoveRange( (&m_data[index]) + 1, &m_data[index], (m_size - index) );
		}
	}

	template<typename T>
	void Vector<T>::RemoveAll( const Vector<T>& to_remove )
	{
		ASSERT( m_size > 0 );

		for( const T& entry : to_remove )
		{
			int index = Find( entry );
			if( index >= 0 )
				Remove( (uint) index );
		}
	}

	template<typename T>
	T Vector<T>::Pop()
	{
		ASSERT( m_size > 0 );

		--m_size;

		T entry = m_data[m_size];

		Zero( m_size );

		return entry;
	}

	template<typename T>
	void Vector<T>::Clear()
	{
		DestroyRange( m_data, m_size );

		memset( m_data, 0xffffffff, sizeof( T ) * m_capacity );

		m_size = 0;
	}

	template<typename T>
	void Vector<T>::Zero( uint index ) const
	{
		ASSERT( index < m_capacity );

		m_data[index].~T();

		memset( &m_data[index], 0xffffffff, sizeof( T ) );
	}

	template<typename T>
	void Vector<T>::Add( T&& entry )
	{
		if( m_size == m_capacity )
		{
			Grow();
		}

		new (&m_data[m_size]) T( entry );

		++m_size;
	}

	template<typename T>
	void Vector<T>::Add( const T& entry )
	{
		if( m_size == m_capacity )
		{
			Grow();
		}

		new (&m_data[m_size]) T( entry );

		++m_size;
	}

	template<typename T>
	void Vector<T>::Push( T&& entry )
	{
		Add( std::move( entry ) );
	}

	template<typename T>
	void Vector<T>::Push( const T& entry )
	{
		Add( entry );
	}

	template<typename T>
	void Vector<T>::Insert( const T&& entry, uint index )
	{
		ASSERT( index <= m_size );

		if( m_size == m_capacity )
		{
			Grow();
		}

		if( index == m_size )
		{
			Add( entry );
		}
		else
		{
			MoveRange( &m_data[index], &m_data[index + 1], m_size - index );

			new (&m_data[index]) T( entry );

			++m_size;
		}
	}

	template<typename T>
	void Vector<T>::Insert( const T& entry, uint index )
	{
		ASSERT( index <= m_size );

		if( m_size == m_capacity )
		{
			Grow();
		}

		if( index == m_size )
		{
			Add( entry );
		}
		else
		{
			MoveRange( &m_data[index], &m_data[index + 1], m_size - index );

			new (&m_data[index]) T( entry );

			++m_size;
		}
	}

	template<typename T>
	void Vector<T>::AddAll( const Vector<T>& other )
	{
		uint new_size = Size() + other.Size();

		Grow( new_size );

		for( uint i = 0; i < other.Size(); ++i )
		{
			m_data[m_size + i] = other[i];
		}

		m_size += other.Size();
	}

	template<typename T>
	T& Vector<T>::Allocate()
	{
		if( m_size == m_capacity )
		{
			Grow();
		}

		T* ptr = new (&m_data[m_size]) T;

		++m_size;

		return *ptr;
	}

	template<typename T>
	T& Vector<T>::Allocate( T&& to_add )
	{
		if( m_size == m_capacity )
		{
			Grow();
		}

		T* ptr = new (&m_data[m_size]) T( std::move( to_add ) );

		++m_size;

		return *ptr;
	}

	template<typename T>
	void Vector<T>::Swap( Vector<T>& other )
	{
		std::swap( m_data, other.m_data );
		std::swap( m_size, other.m_size );
		std::swap( m_capacity, other.m_capacity );
	}

	template<typename T>
	int Vector<T>::Find( const T& entry ) const
	{
		for( uint i = 0; i < m_size; ++i )
		{
			if( m_data[i] == entry )
				return (int) i;
		}

		return -1;
	}

	template<typename T>
	bool Vector<T>::Contains( const T& entry ) const
	{
		return Find( entry ) != -1;
	}

	template<typename T>
	void Vector<T>::Resize( uint size )
	{
		if( size == m_capacity )
			return;

		if( size < m_capacity )
		{
			if( size < m_size )
				DestroyRange( &m_data[size], m_size - size );

			Reallocate( size );
		}
		else if( size > m_capacity )
		{
			Reallocate( size );

			ConstructRange( &m_data[m_size], size - m_size );
		}

		m_size = size;
	}

	template<typename T>
	void Vector<T>::Reserve( uint capacity )
	{
		if( capacity <= m_capacity )
			return;

		Reallocate( capacity );
	}

	template<typename T>
	void Vector<T>::ShrinkToFit()
	{
		Reallocate( m_size );
	}

	template<typename T>
	void Vector<T>::Grow()
	{
		// an unparametrized grow just grows by the default growth factor
		Reallocate( ((uint) (m_capacity * GrowthFactor)) + GrowthFudge );
	}

	template<typename T>
	void Vector<T>::Grow( uint target )
	{
		if( target <= m_capacity )
			return;

		uint new_capacity = m_capacity;
		while( new_capacity < target )
		{
			new_capacity = ((uint) (new_capacity * GrowthFactor)) + GrowthFudge;
		}

		Reallocate( new_capacity );
	}

	template<typename T>
	void Vector<T>::Reallocate( uint new_capacity )
	{
		T* new_data = nullptr;

		if( new_capacity > 0 )
		{
			new_data = reinterpret_cast<T*>(new char[new_capacity * sizeof( T )]);
			memset( new_data, 0xffffffff, new_capacity * sizeof( T ) );
		}

		if( m_data != nullptr )
		{
			CopyRange( m_data, new_data, std::min( m_size, new_capacity ) );
			DestroyRange( m_data, m_size );

			delete[] ((char*) m_data);
		}

		m_data = new_data;
		m_capacity = new_capacity;
	}

	template<typename T>
	T& Vector<T>::GetEntry( uint index ) const
	{
		ASSERT( index < m_capacity );

		return m_data[index];
	}

	template<typename T>
	bool Vector<T>::operator==( const Vector<T>& other ) const
	{
		if( m_size != other.m_size )
			return false;

		for( uint i = 0; i < m_size; ++i )
		{
			if( m_data[i] != other.m_data[i] )
				return false;
		}

		return true;
	}
}