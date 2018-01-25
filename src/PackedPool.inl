#pragma once

namespace dd
{
	template <typename T>
	PackedPool<T>::PackedPool()
	{

	}

	template <typename T>
	PackedPool<T>::PackedPool( PackedPool<T>&& other ) :
		m_entries( other.m_entries ),
		m_components( other.m_components ),
		m_count( other.m_count )
	{

	}

	template <typename T>
	PackedPool<T>::PackedPool( const PackedPool<T>& other ) :
		m_entries( other.m_entries ),
		m_components( other.m_components ),
		m_count( other.m_count )
	{

	}

	template <typename T>
	PackedPool<T>::~PackedPool()
	{
		m_count = 0;
	}

	template <typename T>
	PackedPool<T>& PackedPool<T>::operator=( PackedPool<T>&& other )
	{
		std::swap( m_entries, other.m_entries );
		std::swap( m_components, other.m_components );
		std::swap( m_count, other.m_count );

		return *this;
	}

	template <typename T>
	PackedPool<T>& PackedPool<T>::operator=( const PackedPool<T>& other )
	{
		m_entries = other.m_entries;
		m_components = other.m_components;
		m_count = other.m_count;

		return *this;
	}

	template <typename T>
	void PackedPool<T>::Clear()
	{
		m_entries.clear();
		m_components.clear();
		m_count = 0;
	}

	template <typename T>
	uint PackedPool<T>::Size() const
	{
		return m_count;
	}

	template <typename T>
	T* PackedPool<T>::Create( const EntityHandle& entity )
	{
		if( entity.ID >= m_entries.size() )
		{
			m_entries.resize( entity.ID + 1 );
		}

		ComponentEntry& entry = m_entries[ entity.ID ];
		DD_ASSERT( entry.Handle != entity.Handle, "Create is trying to overwrite a handle that already exists!" );

		entry.Handle = entity.Handle;
		entry.Index = m_count;
		++m_count;

		if( entry.Index >= m_components.size() )
		{
			m_components.resize( entry.Index + 1 );
		}

		return new (&m_components[ entry.Index ]) T();
	}

	template <typename T>
	T* PackedPool<T>::Find( const EntityHandle& entity ) const
	{
		if( entity.ID >= m_entries.size() )
		{
			return nullptr;
		}

		const ComponentEntry& entry = m_entries[ entity.ID ];
		
		DD_ASSERT( entry.Handle == entity.Handle, "Invalid handle found in component pool!" );
		DD_ASSERT( entry.Index != -1, "Component entry has invalid index!" );

		return const_cast<T*>( &m_components[ entry.Index ] );
	}

	template <typename T>
	void PackedPool<T>::Remove( const EntityHandle& entity )
	{
		if( entity.ID >= m_entries.size() )
		{
			return;
		}

		ComponentEntry& entry = m_entries[ entity.ID ];
		
		DD_ASSERT( entry.Handle == entity.Handle, "Invalid handle found in component pool!" );
		DD_ASSERT( entry.Index != -1, "Component entry has invalid index while removing!" );

		--m_count;

		for( ComponentEntry& existing : m_entries )
		{
			if( existing.Index == m_count )
			{
				std::swap( m_components[entry.Index], m_components[existing.Index] );
				existing.Index = entry.Index;
			}
		}

		entry.Index = -1;
		entry.Handle = -1;

		m_components.pop_back();
	}

	template <typename T>
	bool PackedPool<T>::Exists( const EntityHandle& entity ) const
	{
		if( entity.ID >= m_entries.size() )
		{
			return false;
		}

		const ComponentEntry& entry = m_entries[ entity.ID ];
		if( entry.Handle != entity.Handle )
		{
			return false;
		}

		if( entry.Index == -1 )
		{
			return false;
		}

		return true;
	}

	template <typename T>
	typename std::vector<T>::iterator PackedPool<T>::begin()
	{
		return m_components.begin();
	}
	
	template <typename T>
	typename std::vector<T>::iterator PackedPool<T>::end()
	{
		return m_components.end();
	}
	
	template <typename T>
	typename std::vector<T>::const_iterator PackedPool<T>::begin() const
	{
		return m_components.begin();
	}
	
	
	template <typename T>
	typename std::vector<T>::const_iterator PackedPool<T>::end() const
	{
		return m_components.end();
	}
}