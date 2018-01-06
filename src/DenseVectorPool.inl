//
// DenseVectorPool.inl - A pool that holds components in a vector.
// Copyright (C) Sebastian Nordgren 
// October 15th 2015
//

namespace dd
{
	template <typename T>
	class DenseVectorPoolIterator
	{
	private:

		int Index;
		const DenseVectorPool<T>& Pool;

	public:

		DenseVectorPoolIterator( int index, const DenseVectorPool<T>& pool )
			: Index( index ),
			Pool( pool )
		{
		}

		DenseVectorPoolIterator( const DenseVectorPoolIterator<T>& other ) :
			Index( other.Index ),
			Pool( other.Pool )
		{
		}

		const T& operator*() const
		{
			return Pool.m_components[Index];
		}

		T& operator*()
		{
			return Pool.m_components[Index];
		}

		DenseVectorPoolIterator<T> operator++()
		{
			for( ++Index; Index < Pool.m_components.Size(); ++Index )
			{
				if( Pool.IsValid( Index ) )
					return *this;
			}
			
			return *this;
		}

		bool operator!=( const DenseVectorPoolIterator<T>& other ) const
		{
			return &Pool != &other.Pool || Index != other.Index;
		}
	};

	template <typename T>
	bool DenseVectorPool<T>::IsValid( int id ) const
	{
		char c = m_valid[id / (sizeof( char ) * 8)];

		int index = id % (sizeof( char ) * 8);

		return (c >> index) & 1;
	}

	template <typename T>
	void DenseVectorPool<T>::SetValid( int id, bool value )
	{
		char& c = m_valid[id / (sizeof( char ) * 8)];

		int index = id % (sizeof( char ) * 8);

		if( value )
			c |= (1 << index);
		else
			c &= ~(1 << index);
	}

	template <typename T>
	DenseVectorPool<T>::DenseVectorPool()
	{

	}

	template <typename T>
	DenseVectorPool<T>::~DenseVectorPool()
	{
		Clear();
	}

	template <typename T>
	void DenseVectorPool<T>::Clear()
	{
		std::swap( m_components, Vector<T>() );
		std::swap( m_valid, Vector<char>() );
	}

	template <typename T>
	int DenseVectorPool<T>::Size() const
	{
		int count = 0;
		for( int i = 0; i < m_components.Size(); ++i )
		{
			if( IsValid( i ) )
				++count;
		}

		return count;
	}

	template <typename T>
	T* DenseVectorPool<T>::Create( EntityHandle entity )
	{
		// already allocated!
		if( Exists( entity ) )
		{
			DD_ASSERT( false, "Entity already exists!" );
			return nullptr;
		}

		if( entity.ID >= (int) m_components.Size() )
		{
			m_components.Resize( entity.ID + 1 );

			m_valid.Resize( (entity.ID / (sizeof( char ) * 8) ) + 1 );
		}

		SetValid( entity.ID, true );

		T* cmp = new (&m_components[entity.ID]) T();

		IComponent* baseptr = static_cast<IComponent*>(cmp);
		baseptr->Entity = entity;

		return cmp;
	}

	template <typename T>
	template <typename... Args>
	virtual T* DenseVectorPool<T>::Construct( EntityHandle entity, Args&&... args )
	{
		// already allocated!
		if( Exists( entity ) )
		{
			DD_ASSERT( false, "Entity already exists!" );
			return nullptr;
		}

		if( entity.ID >= (int) m_components.Size() )
		{
			m_components.Resize( entity.ID + 1 );

			m_valid.Resize( (entity.ID / (sizeof( char ) * 8)) + 1 );
		}

		SetValid( entity.ID, true );

		T* cmp = new (&m_components[ entity.ID ]) T( std::forward( args )... );

		IComponent* baseptr = static_cast<IComponent*>(cmp);
		baseptr->Entity = entity;

		return cmp;
	}

	template <typename T>
	T* DenseVectorPool<T>::Find( EntityHandle entity ) const
	{
		if( !Exists( entity ) )
		{
			DD_ASSERT( false, "Entity does not exist!" );
			return nullptr;
		}

		T& cmp = m_components[entity.ID];
		return &cmp;
	}

	template <typename T>
	void DenseVectorPool<T>::Remove( EntityHandle entity )
	{
		if( entity.ID >= (int) m_components.Size() )
		{
			DD_ASSERT( false, "Entity ID outside of valid range!" );
			return;
		}

		m_components[entity.ID].~T();

		SetValid( entity.ID, false );
	}

	template <typename T>
	bool DenseVectorPool<T>::Exists( EntityHandle entity ) const
	{
		if( entity.ID >= (int) m_components.Size() )
			return false;

		return IsValid( entity.ID );
	}

	template <typename T>
	DenseVectorPoolIterator<T> DenseVectorPool<T>::begin() const
	{
		for( int i = 0; i < m_components.Size(); ++i )
		{
			if( IsValid( i ) )
				return DenseVectorPoolIterator<T>( i, *this );
		}

		return end();
	}

	template <typename T>
	DenseVectorPoolIterator<T> DenseVectorPool<T>::end() const
	{
		return DenseVectorPoolIterator<T>( m_components.Size(), *this );
	}
}