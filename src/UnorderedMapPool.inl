//
// UnorderedMapPool.inl - Component pool that uses an unordered map for storage.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

namespace dd
{
	template <typename T>
	UnorderedMapPool<T>::UnorderedMapPool()
	{

	}

	template <typename T>
	UnorderedMapPool<T>::~UnorderedMapPool()
	{
		Clear();
	}

	template <typename T>
	void UnorderedMapPool<T>::Clear()
	{
		m_components.swap( std::unordered_map<int, T>() );
	}

	template <typename T>
	uint UnorderedMapPool<T>::Size() const
	{
		return (uint) m_components.size();
	}

	template <typename T>
	T* UnorderedMapPool<T>::Create( const EntityHandle& entity )
	{
		// already allocated!
		T* pCmp = Find( entity );
		if( pCmp != nullptr )
		{
			ASSERT( false, "Component already allocated for given entity!" );
			return nullptr;
		}

		auto it = m_components.insert( std::make_pair( entity.ID, T() ) );
		return &it.first->second;
	}

	template <typename T>
	T* UnorderedMapPool<T>::Find( const EntityHandle& entity ) const
	{
		auto it = m_components.find( entity.ID );
		if( it == m_components.end() )
		{
			return nullptr;
		}

		const T& cmp = it->second;
		return &const_cast<T&>( cmp );
	}

	template <typename T>
	void UnorderedMapPool<T>::Remove( const EntityHandle& entity )
	{
		auto it = m_components.find( entity.ID );
		if( it == m_components.end() )
			return;

		m_components.erase( it );
	}

	template <typename T>
	bool UnorderedMapPool<T>::Exists( const EntityHandle& entity ) const
	{
		T* cmp = Find( entity );

		return cmp != nullptr;
	}

	template <typename T>
	typename UnorderedMapPool<T>::iterator UnorderedMapPool<T>::begin()
	{
		return iterator( m_components.begin() );
	}

	template <typename T>
	typename UnorderedMapPool<T>::iterator UnorderedMapPool<T>::end()
	{
		return iterator( m_components.end() );
	}

	template <typename T>
	typename UnorderedMapPool<T>::const_iterator UnorderedMapPool<T>::begin() const
	{
		return const_iterator( m_components.begin() );
	}

	template <typename T>
	typename UnorderedMapPool<T>::const_iterator UnorderedMapPool<T>::end() const
	{
		return const_iterator( m_components.end() );
	}
}