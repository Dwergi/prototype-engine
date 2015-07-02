#pragma once

#include "ComponentPoolBase.h"
#include "VectorPairIterator.h"

#include <utility>
#include <vector>
#include <set>

//
// A component pool for components that are rare(ish).
// All operations are somewhat more costly than for a dense component pool, but uses less space for large numbers of entities.
//
template<typename T>
class UnsortedVectorPool : public ComponentPoolBase
{
public:

	static_assert( std::is_base_of<Component, T>::value, "Not derived from Component." );

#ifdef USE_EIGEN
	typedef typename aligned_vector<std::pair<int, T>> Storage;
#else
	typedef typename std::vector<std::pair<int, T>> Storage;
#endif 

	typedef typename vector_pair_iterator<int, T> ComponentIterator;

	UnsortedVectorPool()
	{

	}

	~UnsortedVectorPool()
	{
		Cleanup();
	}

	void Cleanup()
	{
		m_components.swap( Storage() );
		m_existence.swap( std::set<int>() );
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
		// already allocated!
		if( Exists( entity ) )
		{
			assert( false );
			return nullptr;
		}

		m_components.push_back( std::make_pair( entity.ID, T() ) );
		m_existence.insert( entity.ID );

		T& cmp = m_components.back().second;
		return &cmp;
	}

	//
	// Find the component for the given entity.
	// Returns null if the component hasn't been created.
	// 
	T* Find( const EntityHandle& entity ) const
	{
		for( auto pair : m_components )
		{
			if( pair.first == entity.ID )
				return &pair.second;
		}

		return nullptr;
	}

	//
	// Remove the component associated with the given entity.
	// 
	void Remove( const EntityHandle& entity )
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
		erase_unordered( m_components, it );
	}

	// 
	// Checks if the given entity has a component of this type.
	// 
	bool Exists( const EntityHandle& entity ) const
	{
		return m_existence.find( entity.ID ) != m_existence.end();
	}

	ComponentIterator begin()
	{
		return ComponentIterator( m_components.begin() );
	}

	ComponentIterator end()
	{
		return ComponentIterator( m_components.end() );
	}

private:

	std::set<int> m_existence;
	Storage m_components;	
};