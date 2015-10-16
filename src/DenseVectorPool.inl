//
// DenseVectorPool.inl - A pool that holds components in a vector.
// Copyright (C) Sebastian Nordgren 
// October 15th 2015
//

namespace dd
{
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
		std::swap( m_valid, Vector<bool>() );
	}

	template <typename T>
	uint DenseVectorPool<T>::Size() const
	{
		uint count = 0;
		for( bool valid : m_valid )
		{
			if( valid )
				++count;
		}

		return count;
	}

	template <typename T>
	T* DenseVectorPool<T>::Create( const EntityHandle& entity )
	{
		// already allocated!
		if( Exists( entity ) )
		{
			ASSERT( false, "Entity already exists!" );
			return nullptr;
		}

		if( entity.ID < 0 || entity.ID >= (int) m_components.Size() )
		{
			m_components.Resize( entity.ID + 1 );
			m_valid.Resize( entity.ID + 1 );
		}

		m_valid[entity.ID] = true;

		T* cmp = new (&m_components[entity.ID]) T();
		return cmp;
	}
	
	template <typename T>
	T* DenseVectorPool<T>::Find( const EntityHandle& entity ) const
	{
		if( !Exists( entity ) )
		{
			ASSERT( false, "Entity does not exist!" );
			return nullptr;
		}

		T& cmp = m_components[entity.ID];
		return &cmp;
	}

	template <typename T>
	void DenseVectorPool<T>::Remove( const EntityHandle& entity )
	{
		if( entity.ID < 0 || entity.ID >= (int) m_valid.Size() )
		{
			ASSERT( false, "Entity ID outside of valid range!" );
			return;
		}

		m_components[entity.ID].~T();
		m_valid[entity.ID] = false;
	}

	template <typename T>
	bool DenseVectorPool<T>::Exists( const EntityHandle& entity ) const
	{
		if( entity.ID < 0 || entity.ID >= (int) m_valid.Size() )
			return false;

		bool isValid = m_valid[entity.ID];
		return isValid;
	}

	template <typename T>
	Iterator<T> DenseVectorPool<T>::begin() const
	{
		return m_components.begin();
	}

	template <typename T>
	Iterator<T> DenseVectorPool<T>::end() const
	{
		return m_components.end();
	}
}