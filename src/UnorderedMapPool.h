//
// UnorderedMapPool.h - Component pool that uses an unordered map for storage.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "ComponentPoolBase.h"
#include "EntityHandle.h"

template<typename T>
class UnorderedMapPool;

template<typename TKey, typename TValue>
class unordered_map_pair_const_iterator
	: std::iterator<std::forward_iterator_tag, TValue*>
{
private:
	typedef unordered_map_pair_const_iterator<TKey, TValue> this_type;
	typedef typename std::unordered_map<TKey, TValue>::const_iterator std_pair_iter;

	std_pair_iter m_current;

public:

	unordered_map_pair_const_iterator( std_pair_iter& init )
	{
		m_current = init;
	}

	unordered_map_pair_const_iterator( const this_type& other )
	{
		m_current = other.m_current;
	}

	const TValue* operator&()
	{
		return &m_current->second;
	}

	const TValue& operator*() 
	{ 
		return m_current->second;
	}
	
	bool operator==( const this_type& other ) const
	{
		return m_current == other.m_current;
	}

	bool operator!=( const this_type& other ) const
	{
		return !operator==( other );
	}

	this_type& operator++()
	{
		++m_current;

		return *this;
	}
};

//
// Pair iterator.
// 
template<typename TKey, typename TValue>
class unordered_map_pair_iterator
	: std::iterator<std::forward_iterator_tag, TValue*>
{
private:
	typedef unordered_map_pair_iterator<TKey, TValue> this_type;
	typedef typename std::unordered_map<TKey, TValue>::iterator std_pair_iter;

	std_pair_iter m_current;

public:

	unordered_map_pair_iterator( std_pair_iter& init )
	{
		m_current = init;
	}

	unordered_map_pair_iterator( const this_type& other )
	{
		m_current = other.m_current;
	}

	TValue* operator&()
	{
		return &m_current->second;
	}

	TValue& operator*() 
	{ 
		return m_current->second;
	}

	const TValue* operator&() const
	{
		return &m_current->second;
	}

	const TValue& operator*() const
	{ 
		return m_current->second;
	}

	bool operator==( const this_type& other ) const
	{
		return m_current == other.m_current;
	}

	bool operator!=( const this_type& other ) const
	{
		return !operator==( other );
	}

	this_type& operator++()
	{
		++m_current;

		return *this;
	}
};

//
// A component pool for components that are rare(ish).
// All operations are somewhat more costly than for a dense component pool, but uses less space for large numbers of entities.
//
namespace dd
{
	template<typename T>
	class UnorderedMapPool : public ComponentPoolBase
	{
	public:

		static_assert( std::is_base_of<dd::Component, T>::value, "Not derived from Component." );

	#ifdef USE_EIGEN
		typedef typename std::unordered_map<int, T, std::hash<int>, std::equal_to<int>, Eigen::aligned_allocator< std::pair<int, T> > > Storage;
	#else
		typedef typename std::unordered_map<int, T> Storage;
	#endif

		typedef typename unordered_map_pair_iterator<int, T> ComponentIterator;
		typedef typename unordered_map_pair_const_iterator<int, T> ComponentConstIterator;

		UnorderedMapPool()
		{

		}

		~UnorderedMapPool()
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
		T* Create( const dd::EntityHandle& entity )
		{
			// already allocated!
			T* pCmp = Find( entity );
			if( pCmp != nullptr )
			{
				ASSERT( false, "Component already allocated for given entity!" );
				return nullptr;
			}

			auto res = m_components.insert( std::make_pair( entity.ID, T() ) );
			return &res.first->second;
		}

		//
		// Find the component for the given entity.
		// Returns null if the component hasn't been created.
		// 
		T* Find( const dd::EntityHandle& entity )
		{
			auto it = m_components.find( entity.ID );
			if( it == m_components.end() )
			{
				return nullptr;
			}

			T& cmp = it->second;
			return &cmp;
		}

		//
		// Remove the component associated with the given entity.
		// 
		void Remove( const dd::EntityHandle& entity )
		{
			auto it = m_components.find( entity.ID );
			if( it == m_components.end() )
				return;

			m_components.erase( it );
		}

		// 
		// Checks if the given entity has a component of this type.
		// 
		bool Exists( const dd::EntityHandle& entity ) const
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

		ComponentConstIterator begin() const
		{
			return ComponentConstIterator( m_components.begin() );
		}

		ComponentConstIterator end() const
		{
			return ComponentConstIterator( m_components.end() );
		}

		void operator=( const UnorderedMapPool<T>& other )
		{
			m_components = other.m_components;
		}

	private:

		Storage m_components;	
	};
}