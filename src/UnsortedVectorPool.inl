//
// UnsortedVectorPool.inl - A completely naive pool. Minimal memory footprint, fast adds, slow finds/deletes.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

namespace dd
{
	template <typename T>
	UnsortedVectorPool<T>::UnsortedVectorPool()
	{

	}

	template <typename T>
	UnsortedVectorPool<T>::~UnsortedVectorPool()
	{
		Clear();
	}

	template <typename T>
	void UnsortedVectorPool<T>::Clear()
	{
		m_components.swap( Storage() );
		m_existence.swap( std::set<int>() );
	}

	template <typename T>
	uint UnsortedVectorPool<T>::Size() const
	{
		return (uint) m_existence.size();
	}

	template <typename T>
	T* UnsortedVectorPool<T>::Create( const EntityHandle& entity )
	{
		// already allocated!
		if( Exists( entity ) )
		{
			ASSERT( false );
			return nullptr;
		}

		m_components.push_back( std::make_pair( entity.ID, T() ) );
		m_existence.insert( entity.ID );

		T& cmp = m_components.back().second;
		return &cmp;
	}

	template <typename T>
	T* UnsortedVectorPool<T>::Find( const EntityHandle& entity ) const
	{
		for( auto& pair : m_components )
		{
			if( pair.first == entity.ID )
				return const_cast<T*>( &pair.second );
		}

		return nullptr;
	}

	template <typename T>
	void UnsortedVectorPool<T>::Remove( const EntityHandle& entity )
	{
		if( !Exists( entity ) )
			return;

		auto it = m_components.begin();
		for( ; it != m_components.end(); ++it )
		{
			if( it->first == entity.ID )
				break;
		}

		if( it == m_components.end() )
			return;

		m_existence.erase( m_existence.find( it->first ) );
		m_components.erase( it );
	}

	template <typename T>
	bool UnsortedVectorPool<T>::Exists( const EntityHandle& entity ) const
	{
		return m_existence.find( entity.ID ) != m_existence.end();
	}

	template <typename T>
	typename UnsortedVectorPool<T>::iterator UnsortedVectorPool<T>::begin()
	{
		return iterator( m_components.begin() );
	}

	template <typename T>
	typename UnsortedVectorPool<T>::iterator UnsortedVectorPool<T>::end()
	{
		return iterator( m_components.end() );
	}

	template <typename T>
	typename UnsortedVectorPool<T>::const_iterator UnsortedVectorPool<T>::begin() const
	{
		return const_iterator( m_components.begin() );
	}

	template <typename T>
	typename UnsortedVectorPool<T>::const_iterator UnsortedVectorPool<T>::end() const
	{
		return const_iterator( m_components.end() );
	}
}