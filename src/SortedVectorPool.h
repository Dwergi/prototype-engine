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
namespace dd
{
	template <typename T>
	class SortedVectorPoolIterator;

	template <typename T>
	class SortedVectorPool
		: public ComponentPoolBase
	{
	private:
		struct EntityEntry
		{
			uint Entity;
			T Component;
		};

	public:

		typedef typename SortedVectorPoolIterator<T> iterator;

		SortedVectorPool();
		~SortedVectorPool();

		void Clear();

		//
		// Checks if this component pool is empty or not.
		// 
		uint Size() const;

		//
		// Create a new component of this type for the given entity.
		// 
		T* Create( const EntityHandle& entity );

		//
		// Find the component for the given entity.
		// Returns null if the component hasn't been created.
		// 
		T* Find( const EntityHandle& entity ) const;

		//
		// Remove the component associated with the given entity.
		// 
		void Remove( const EntityHandle& entity );

		// 
		// Checks if the given entity has a component of this type.
		// 
		bool Exists( const EntityHandle& entity ) const;

		iterator begin() const;
		iterator end() const;

		BASIC_TYPE( SortedVectorPool<T> )

	private:

		Vector<EntityEntry> m_components;

		template <typename T>
		friend class SortedVectorPoolIterator;
	};
}

#include "SortedVectorPool.inl"