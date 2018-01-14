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
			for( ++Index; Index < Pool.m_components.size(); ++Index )
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
		std::lock_guard<std::mutex> lock( m_mutex );

		std::swap( m_components, std::vector<T>() );
		std::swap( m_valid, std::vector<bool>() );
	}

	template <typename T>
	int DenseVectorPool<T>::Size() const
	{
		std::lock_guard<std::mutex> lock( m_mutex );

		int count = 0;
		for( int i = 0; i < m_components.size(); ++i )
		{
			if( m_valid[ i ] )
				++count;
		}

		return count;
	}

	template <typename T>
	T* DenseVectorPool<T>::Create( EntityHandle entity )
	{
		std::lock_guard<std::mutex> lock( m_mutex );

		// already allocated!
		if( Exists( entity ) )
		{
			DD_ASSERT( false, "Entity already exists!" );
			return nullptr;
		}

		if( entity.ID >= (int) m_components.size() )
		{
			m_components.resize( entity.ID + 1 );
			m_valid.resize( entity.ID + 1 );
		}

		m_valid[ entity.ID ] = true;

		T* cmp = new (&m_components[entity.ID]) T();

		IComponent* baseptr = static_cast<IComponent*>(cmp);
		baseptr->Entity = entity;

		return cmp;
	}

	template <typename T>
	template <typename... Args>
	virtual T* DenseVectorPool<T>::Construct( EntityHandle entity, Args&&... args )
	{
		std::lock_guard<std::mutex> lock( m_mutex );

		// already allocated!
		if( Exists( entity ) )
		{
			DD_ASSERT( false, "Entity already exists!" );
			return nullptr;
		}

		int size = m_components.size();

		if( entity.ID >= (int) m_components.size() )
		{
			m_components.resize( entity.ID + 1 ); 
			m_valid.resize( entity.ID + 1 );
		}

		for( int i = size; i < m_components.size(); ++i )
		{
			m_valid[i] = false;
		}

		m_valid[entity.ID] = true;

		T* cmp = new (&m_components[entity.ID]) T( std::forward( args )... );

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

		const T& cmp = m_components[entity.ID];
		return &const_cast<T&>( cmp );
	}

	template <typename T>
	void DenseVectorPool<T>::Remove( EntityHandle entity )
	{
		std::lock_guard<std::mutex> lock( m_mutex );

		if( entity.ID >= (int) m_components.size() )
		{
			DD_ASSERT( false, "Entity ID outside of valid range!" );
			return;
		}

		m_components[entity.ID].~T();
		m_valid[entity.ID] = false;
	}

	template <typename T>
	bool DenseVectorPool<T>::Exists( EntityHandle entity ) const
	{
		if( entity.ID >= (int) m_valid.size() )
			return false;

		return m_valid[entity.ID];
	}

	template <typename T>
	DenseVectorPool<T>& DenseVectorPool<T>::operator=( const DenseVectorPool<T>& other )
	{
		std::lock_guard<std::mutex> lock( m_mutex );

		m_components = other.m_components;
		m_valid = other.m_valid;

		return *this;
	}

	template <typename T>
	DenseVectorPoolIterator<T> DenseVectorPool<T>::begin() const
	{
		for( int i = 0; i < m_components.size(); ++i )
		{
			if( m_valid[entity.ID] )
				return DenseVectorPoolIterator<T>( i, *this );
		}

		return end();
	}

	template <typename T>
	DenseVectorPoolIterator<T> DenseVectorPool<T>::end() const
	{
		return DenseVectorPoolIterator<T>( m_components.size(), *this );
	}
}