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
		: public ComponentPool<T>
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

		virtual void Clear() override;

		//
		// Checks if this component pool is empty or not.
		// 
		virtual uint Size() const override;

		//
		// Create a new component of this type for the given entity.
		// 
		virtual T* Create( const EntityHandle& entity ) override;

		//
		// Find the component for the given entity.
		// Returns null if the component hasn't been created.
		// 
		virtual T* Find( const EntityHandle& entity ) const override;

		//
		// Remove the component associated with the given entity.
		// 
		virtual void Remove( const EntityHandle& entity ) override;

		// 
		// Checks if the given entity has a component of this type.
		// 
		virtual bool Exists( const EntityHandle& entity ) const override;

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