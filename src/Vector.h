//
// Vector.h - A vector.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	class VectorBase
	{
	public:
		VectorBase();
		uint Size() const;

	protected:
		uint m_capacity;
		uint m_size;

		static const uint DefaultSize = 8;

		// growth factor and growth fudge defines 
		static const float GrowthFactor;
		static const uint GrowthFudge;
	};

	template< typename T >
	class Vector : public VectorBase
	{
	public:

		Vector();
		Vector( uint reserved_size );
		Vector( const Vector& other );
		Vector( Vector&& other );

		Vector<T>& operator=( const Vector& other );
		T& operator[]( uint index ) const;

		//
		// Remove from the given index in an unordered way. 
		// Swaps the last element with the given index.
		//
		void Remove( uint index );
		void Remove( int index );

		void RemoveItem( const T& item );

		//
		// Remove from the given index in an ordered way.
		// This will invalidate indices or pointers into the vector after the given index.
		//
		void RemoveOrdered( uint index );
		void RemoveAll( const Vector<T>& to_remove );
		void Clear();
		T Pop();

		//
		// Zero the entry by calling the destructor on it. 
		// Extremely unsafe, because this does not decrement the size of the container. 
		// You're basically on your own at this point.
		//
		void Zero( uint index ) const;

		void Add( T&& value );
		void Add( const T& value );
		void Push( T&& value );
		void Push( const T& value );

		//
		// Insert at the given index. 
		// Index must be <= Size()
		//
		void Insert( const T&& value, uint index );

		//
		// Add all the entries from the given vector to the end of this vector.
		//
		void AddAll( const Vector<T>& other );

		//
		// Create a new entry at the back and return a reference to it. 
		//
		T& Allocate();
		T& Allocate( T&& entry );

		void Swap( Vector<T>& other );

		int Find( const T& entry ) const;
		bool Contains( const T& entry ) const;

		void Resize( uint capacity );
		void ShrinkToFit();

		bool operator==( const Vector<T>& other ) const;

		//
		// Iteration
		//
		DEFINE_ITERATORS( T, m_data, m_size )

	private:

		// 
		// Grows the vector to contain at least target elements.
		// The actual capacity will depend on the growth factor.
		//
		void Grow( uint target );
		void Grow();

		void Reallocate( uint new_capacity );

		T& GetEntry( uint index ) const;

		T* m_data;
	};

	//
	// Function definitions follow...
	//

	template<typename T> 
	Vector<T>::Vector()
		: m_data( nullptr )
	{
		// we need to register this type so we have serializers available
	}

	template<typename T> 
	Vector<T>::Vector( uint reserved )
		: m_data( nullptr )
	{
		Resize( reserved );
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
		new (&m_data[ index ]) T( m_data[ m_size ] );
		
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
			MoveRange( (&m_data[ index ]) + 1, &m_data[ index ], (m_size - index) );
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

		T entry = m_data[ m_size ];

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
		
		m_data[ index ].~T();

		memset( &m_data[ index ], 0xffffffff, sizeof( T ) );
	}

	template<typename T>
	void Vector<T>::Add( T&& entry )
	{
		if( m_size == m_capacity )
		{
			Grow();
		}

		new (&m_data[ m_size ]) T( entry );

		++m_size;
	}

	template<typename T>
	void Vector<T>::Add( const T& entry )
	{
		if( m_size == m_capacity )
		{
			Grow();
		}

		new (&m_data[ m_size ]) T( entry );

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

		if( index == m_size )
		{
			Add( entry );
		}
		else
		{
			MoveRange( &m_data[ index ], &m_data[ index ] + 1, m_size - index );

			new (&m_data[ m_size ]) T( entry );
		}
	}

	template<typename T>
	void Vector<T>::AddAll( const Vector<T>& other )
	{
		uint new_size = Size() + other.Size();

		Grow( new_size );

		for( uint i = 0; i < other.Size(); ++i )
		{
			m_data[ m_size + i ] = other[ i ];
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

		T* ptr = new (&m_data[ m_size ]) T;

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

		T* ptr = new (&m_data[ m_size ]) T( std::move( to_add ) );

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
			if( m_data[ i ] == entry )
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
				DestroyRange( &m_data[ size ], m_size - size );

			m_size = size;

			Reallocate( size );
		}

		if( size > m_capacity )
		{
			Reallocate( size );
		}
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
		if( target < m_capacity )
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
		if( new_capacity == 0 )
			return;

		T* new_data = reinterpret_cast<T*>( new char[ new_capacity * sizeof( T ) ] );

		memset( new_data, 0xffffffff, new_capacity * sizeof( T ) );

		if( m_data != nullptr )
		{
			CopyRange( m_data, new_data, std::min( m_size, new_capacity ) );
			DestroyRange( m_data, m_size );

			delete[] (char*) m_data;
		}

		m_data = new_data;
		m_capacity = new_capacity;
	}

	template<typename T>
	T& Vector<T>::GetEntry( uint index ) const
	{
		ASSERT( index < m_size );

		return m_data[ index ];
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