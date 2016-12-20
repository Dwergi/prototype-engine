//
// DenseVectorPool.h - A pool that holds components in a vector.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "ComponentPoolBase.h"

namespace dd
{
	template <typename T>
	class DenseVectorPoolIterator;

	//
	// A dense component pool is one for which the component is assumed to exist for all entities (eg. transform).
	// Uses much more space than a sparse component pool for large numbers of entities.
	//
	template <typename T>
	class DenseVectorPool 
		: public ComponentPoolBase
	{
		static_assert(std::is_base_of<ComponentBase, T>::value, "Not derived from Component.");

	public:
		typedef DenseVectorPoolIterator<T> iterator;

		DenseVectorPool();
		~DenseVectorPool();

		//
		// Clear the entire component pool.
		//
		void Clear();

		//
		// Checks if this component pool is empty or not.
		// 
		uint Size() const;

		//
		// Create a new component of this type for the given entity and return the pointer to it.
		// Returns null if the component already exists.
		// 
		T* Create( const EntityHandle& entity );

		//
		// Construct a new component of this type for the given entity with the given arguments.
		// Returns the pointer to it or null if the component already exists.
		//
		template <typename... Args>
		T* Construct( const EntityHandle& entity, Args&&... args );

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

		BASIC_TYPE( DenseVectorPool<T> )

	private:

		template <typename T>
		friend class DenseVectorPoolIterator;

		Vector<T> m_components;
		Vector<char> m_valid; // this is actually a bitmap

		bool IsValid( uint id ) const;
		void SetValid( uint id, bool value );
	};
}

#include "DenseVectorPool.inl"