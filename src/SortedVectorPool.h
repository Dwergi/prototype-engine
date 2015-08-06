//
// SortedVectorPool.h - A pool for components that is kept sorted and uses binary search to find entries.
// Performs atrociously for adds/removes.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "ComponentPoolBase.h"

//
// A component pool for components that are rare(ish).
// All operations are somewhat more costly than for a dense component pool, but uses less space for large numbers of entities.
//
template<typename T>
class SortedVectorPool : public ComponentPoolBase
{
public:

	static_assert( std::is_base_of<Component, T>::value, "Not derived from Component." );

	typedef typename std::vector<std::pair<int, T>> Storage;

	typedef typename vector_pair_iterator<int, T> ComponentIterator;

	SortedVectorPool()
	{

	}

	~SortedVectorPool()
	{
		Cleanup();
	}

	void Cleanup()
	{
		m_components.swap( Storage() );
	}

	//
	// Checks if this component pool is empty or not.
	// 
	bool Empty() const
	{
		return m_components.empty();
	}

	//
	// Create a new component of this type for the given entity.
	// 
	T* Create( const EntityHandle& entity )
	{
		// use insertion with a binary search to make sure we keep the list in order
		int min = 0;
		int max = m_components.size();

		Storage::iterator it = m_components.begin();
		while( max >= min )
		{
			int mid = (min + max) / 2;

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

	//
	// Find the component for the given entity.
	// Returns null if the component hasn't been created.
	// 
	T* Find( const EntityHandle& entity )
	{
		int min = 0;
		int max = m_components.size();

		Storage::iterator it = m_components.begin();
		while( max >= min )
		{
			int mid = (min + max) / 2;

			auto& entry = m_components[ mid ];

			if( entry.first == entity.ID )
			{
				return &entry.second;
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

	//
	// Remove the component associated with the given entity.
	// 
	void Remove( const EntityHandle& entity )
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

	// 
	// Checks if the given entity has a component of this type.
	// 
	bool Exists( const EntityHandle& entity ) const
	{
		T* cmp = Find( entity );

		return cmp != nullptr;
	}

	ComponentIterator begin()
	{
		return ComponentIterator( m_components.begin() );
	}

	ComponentIterator end()
	{
		return ComponentIterator( m_components.end() );
	}

	bool Validate()
	{
		int previous = INT_MIN;

		for( auto entry : m_components )
		{
			if( previous > entry.first )
				return false;

			previous = entry.first;
		}

		return true;
	}

private:

	Storage m_components;

	typename Storage::iterator FindIterator( const EntityHandle& entity )
	{
		
	}
};