//
// DenseVectorPool.h - A pool that holds components in a vector.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "ComponentPoolBase.h"

namespace dd
{
	//
	// A dense component pool is one for which the component is assumed to exist for all entities (eg. transform).
	// Uses much more space than a sparse component pool for large numbers of entities.
	//
	template<typename T>
	class DenseVectorPool 
		: public ComponentPool<T>
	{
		static_assert(std::is_base_of<Component, T>::value, "Not derived from Component.");

	public:
		DenseVectorPool();
		~DenseVectorPool();

		//
		// Clear the entire component pool.
		//
		virtual void Clear() override;

		//
		// Checks if this component pool is empty or not.
		// 
		virtual uint Size() const override;

		//
		// Create a new component of this type for the given entity and return the pointer to it.
		// Returns null if the component already exists.
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

		//
		// Support for iteration over the pool.
		//
		Iterator<T> begin() const;
		Iterator<T> end() const;

	private:

		Vector<T> m_components;
		Vector<char> m_valid; // this is actually a bitmap

		bool IsValid( uint id ) const;
		void SetValid( uint id, bool value );
	};
}

#include "DenseVectorPool.inl"