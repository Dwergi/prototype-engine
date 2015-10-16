//
// SortedVectorPool.inl - A pool for components that is kept sorted and uses binary search to find entries.
// Performs atrociously for adds/removes.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

namespace dd
{
	template <typename T>
	SortedVectorPool<T>::SortedVectorPool()
	{

	}

	template <typename T>
	SortedVectorPool<T>::~SortedVectorPool()
	{
		Clear();
	}

	template <typename T>
	void SortedVectorPool<T>::Clear()
	{
		m_components.swap( std::vector<std::pair<int, T>>() );
	}

	template <typename T>
	uint SortedVectorPool<T>::Size() const
	{
		return (uint) m_components.size();
	}

	template <typename T>
	T* SortedVectorPool<T>::Create( const EntityHandle& entity )
	{
		// use insertion with a binary search to make sure we keep the list in order
		size_t min = 0;
		size_t max = m_components.size();

		auto it = m_components.begin();
		while( max >= min )
		{
			size_t mid = (min + max) / 2;

			it = m_components.begin();
			it += mid;

			if( it == m_components.end() )
				break;

			if( it->first == entity.ID )
			{
				// already exists
				ASSERT( false );
				return nullptr;
			}

			if( it->first < entity.ID )
			{
				min = mid + 1;
			}
			else if( it->first >= entity.ID )
			{
				max = mid - 1;
			}
		}

		// undershot by 1 slot, it happens
		if( it != m_components.begin() && it != m_components.end()
			&& it->first < entity.ID )
			++it;

		auto inserted = m_components.insert( it, std::make_pair( entity.ID, T() ) );

		return &inserted->second;
	}

	template <typename T>
	T* SortedVectorPool<T>::Find( const EntityHandle& entity ) const
	{
		int min = 0;
		int max = (int) m_components.size();

		// empty vector
		if( max == 0 )
			return nullptr;

		while( max >= min )
		{
			int mid = (min + max) / 2;
			auto& entry = m_components[mid];

			if( entry.first == entity.ID )
			{
				return const_cast<T*>( &entry.second );
			}

			if( entry.first < entity.ID )
			{
				min = mid + 1;
			}
			else if( entry.first >= entity.ID )
			{
				max = mid - 1;
			}
		}

		return nullptr;
	}

	template <typename T>
	void SortedVectorPool<T>::Remove( const EntityHandle& entity )
	{
		auto it = m_components.begin();
		for( ; it != m_components.end(); ++it )
		{
			if( it->first == entity.ID )
				break;
		}

		if( it == m_components.end() )
			return;

		m_components.erase( it );
	}

	template <typename T>
	bool SortedVectorPool<T>::Exists( const EntityHandle& entity ) const
	{
		T* cmp = Find( entity );

		return cmp != nullptr;
	}

	template <typename T>
	typename SortedVectorPool<T>::iterator SortedVectorPool<T>::begin()
	{
		return iterator( m_components.begin() );
	}

	template <typename T>
	typename SortedVectorPool<T>::iterator SortedVectorPool<T>::end()
	{
		return iterator( m_components.end() );
	}

	template <typename T>
	typename SortedVectorPool<T>::const_iterator SortedVectorPool<T>::begin() const
	{
		return const_iterator( m_components.begin() );
	}

	template <typename T>
	typename SortedVectorPool<T>::const_iterator SortedVectorPool<T>::end() const
	{
		return const_iterator( m_components.end() );
	}
}