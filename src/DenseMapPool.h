//
// DenseMapPool.h - Component pool that uses an unordered map for storage.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "ComponentPoolBase.h"
#include "ComponentPairIterator.h"
#include "DenseMap.h"

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
		: public ComponentPool<T>
	{
	public:

		DenseMapPool();
		DenseMapPool( DenseMapPool<T>&& other );
		DenseMapPool( const DenseMapPool<T>& other );
		~DenseMapPool();

		DenseMapPool<T>& operator=( DenseMapPool<T>&& other );
		DenseMapPool<T>& operator=( const DenseMapPool<T>& other );


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

		DenseMapPoolIterator<T> begin() const;
		DenseMapPoolIterator<T> end() const;

	private:

		DenseMap<int, T> m_components;
	};
}

#include "DenseMapPool.inl"
