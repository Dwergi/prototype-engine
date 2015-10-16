//
// SortedVectorPool.h - A pool for components that is kept sorted and uses binary search to find entries.
// Performs atrociously for adds/removes.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "ComponentPoolBase.h"
#include "ComponentPairIterator.h"

//
// A component pool for components that are rare(ish).
// All operations are somewhat more costly than for a dense component pool, but uses less space for large numbers of entities.
//
namespace dd
{
	template<typename T>
	class SortedVectorPool
		: public ComponentPool<T>
	{
	private:
		typedef typename ComponentPairIterator<typename std::vector<std::pair<int, T>>::iterator, int, T> iterator;
		typedef typename ComponentPairConstIterator<typename std::vector<std::pair<int, T>>::const_iterator, int, T> const_iterator;

	public:

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

		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;

	private:

		std::vector<std::pair<int, T>> m_components;
	};
}

#include "SortedVectorPool.inl"