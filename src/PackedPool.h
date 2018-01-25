//
// PackedPool.h - A tightly-packed twice-indirect pool.
// Copyright (C) Sebastian Nordgren 
// January 25th 2018
//

#pragma once

#include "IComponentPool.h"

#include <type_traits>

namespace dd
{
	template <typename T>
	class PackedPool : IComponentPool
	{
	public:

		PackedPool();
		PackedPool( PackedPool<T>&& other );
		PackedPool( const PackedPool<T>& other );
		virtual ~PackedPool();

		PackedPool<T>& operator=( PackedPool<T>&& other );
		PackedPool<T>& operator=( const PackedPool<T>& other );

		void Clear();

		uint Size() const;

		T* Create( const EntityHandle& entity );
		T* Find( const EntityHandle& entity ) const;

		virtual void Remove( const EntityHandle& entity ) override;
		virtual bool Exists( const EntityHandle& entity ) const override;

		typename std::vector<T>::iterator begin();
		typename std::vector<T>::iterator end();

		typename std::vector<T>::const_iterator begin() const;
		typename std::vector<T>::const_iterator end() const;

		BASIC_TYPE( PackedPool<T> );

	private:
		
		struct ComponentEntry
		{
			int Handle { -1 };
			int Index { -1 };
		};

		uint m_count { 0 };

		std::vector<ComponentEntry> m_entries;
		std::vector<T> m_components;
	};
}

#include "PackedPool.inl"