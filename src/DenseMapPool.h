//
// DenseMapPool.h - Component pool that uses an unordered map for storage.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "ComponentPoolBase.h"
#include "ComponentPairIterator.h"
#include "DenseMap.h"

#include <unordered_map>

//
// A component pool for components that are rare(ish).
// All operations are somewhat more costly than for a dense component pool, but uses less space for large numbers of entities.
//
namespace dd
{
	template <typename T>
	class DenseMapPoolIterator;

	template <typename T>
	class DenseMapPool
		: public ComponentPoolBase
	{
	public:

		typedef DenseMapPoolIterator<T> iterator;

		DenseMapPool();
		DenseMapPool( DenseMapPool<T>&& other );
		DenseMapPool( const DenseMapPool<T>& other );
		~DenseMapPool();

		DenseMapPool<T>& operator=( DenseMapPool<T>&& other );
		DenseMapPool<T>& operator=( const DenseMapPool<T>& other );

		//
		// Clear this component pool.
		//
		virtual void Clear();

		//
		// Checks if this component pool is empty or not.
		// 
		uint Size() const;

		//
		// Create a new component of this type for the given entity.
		// 
		T* Create( EntityHandle entity );

		//
		// Find the component for the given entity.
		// Returns null if the component hasn't been created.
		// 
		T* Find( EntityHandle entity ) const;

		//
		// Remove the component associated with the given entity.
		// 
		virtual void Remove( EntityHandle entity ) override;

		// 
		// Checks if the given entity has a component of this type.
		// 
		virtual bool Exists( EntityHandle entity ) const override;

		iterator begin();
		iterator end();

		BASIC_TYPE( DenseMapPool<T> )

	private:

		std::mutex m_mutex;
		std::unordered_map<int, T> m_components;
	};
}

#include "DenseMapPool.inl"
