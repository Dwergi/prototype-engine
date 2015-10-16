//
// UnorderedMapPool.h - Component pool that uses an unordered map for storage.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "ComponentPoolBase.h"
#include "ComponentPairIterator.h"

#include <unordered_map>

//
// A component pool for components that are rare(ish).
// All operations are somewhat more costly than for a dense component pool, but uses less space for large numbers of entities.
//
namespace dd
{
	template<typename T>
	class UnorderedMapPool 
		: public ComponentPool<T>
	{
	public:

		typedef typename ComponentPairIterator<typename std::unordered_map<int,T>::iterator, int, T> iterator;
		typedef typename ComponentPairConstIterator<typename std::unordered_map<int, T>::const_iterator, int, T> const_iterator;

		UnorderedMapPool();
		~UnorderedMapPool();

		//
		// Clear this component pool.
		//
		virtual void Clear() override;

		//
		// Checks if this component pool is empty or not.
		// 
		uint Size() const override;

		//
		// Create a new component of this type for the given entity.
		// 
		T* Create( const EntityHandle& entity ) override;

		//
		// Find the component for the given entity.
		// Returns null if the component hasn't been created.
		// 
		T* Find( const EntityHandle& entity ) const override;

		//
		// Remove the component associated with the given entity.
		// 
		void Remove( const EntityHandle& entity ) override;

		// 
		// Checks if the given entity has a component of this type.
		// 
		bool Exists( const EntityHandle& entity ) const override;

		iterator begin();
		iterator end();

		const_iterator begin() const;
		const_iterator end() const;

	private:

		std::unordered_map<int, T> m_components;
	};
}

#include "UnorderedMapPool.inl"
