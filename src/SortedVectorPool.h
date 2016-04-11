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
	template <typename TIter, typename TValue>
	class SortedVectorIterator
		: public std::iterator<std::forward_iterator_tag, TValue*>
	{
	private:
		TIter m_current;
		typedef SortedVectorIterator<TIter, TValue> ThisType;

	public:
		SortedVectorIterator( TIter init );
		SortedVectorIterator( const ThisType& other );

		TValue& operator*() const;
		bool operator==( const ThisType& other );
		bool operator!=( const ThisType& other );
		ThisType& operator++();
	};

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

		typedef typename SortedVectorIterator<typename Vector<EntityEntry>::iterator, T> iterator;

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

		iterator begin() const;
		iterator end() const;

		BASIC_TYPE( SortedVectorPool<T> )

	private:

		Vector<EntityEntry> m_components;
	};
}

#include "SortedVectorPool.inl"