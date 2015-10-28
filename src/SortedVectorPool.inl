//
// SortedVectorPool.inl - A pool for components that is kept sorted and uses binary search to find entries.
// Performs atrociously for adds/removes.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "EntityHandle.h"
#include "Vector.h"

namespace dd
{
	template <typename T>
	SortedVectorPool<T>::SortedVectorPool()
	{

	}

	template <typename T>
	SortedVectorPool<T>::~SortedVectorPool()
	{
		Clear();
	}

	template <typename T>
	void SortedVectorPool<T>::Clear()
	{
		m_components.Clear();
	}

	template <typename T>
	uint SortedVectorPool<T>::Size() const
	{
		return m_components.Size();
	}

	template <typename T>
	T* SortedVectorPool<T>::Create( const EntityHandle& entity )
	{
		// use insertion with a binary search to make sure we keep the list in order
		int min = 0;
		int max = (int) m_components.Size();

		int current = 0;

		while( max >= min )
		{
			current = (min + max) / 2;

			if( current == m_components.Size() )
				break;

			uint current_handle = m_components[current].Entity;

			if( current_handle == entity.Handle )
			{
				// already exists
				ASSERT( false );
				return nullptr;
			}

			if( current_handle < entity.Handle )
			{
				min = current + 1;
			}
			else if( current_handle >= entity.Handle )
			{
				max = current - 1;
			}
		}

		// undershot by 1 slot, it happens
		if( current >= 0 && current < (int) m_components.Size() && m_components[current].Entity < entity.Handle )
			++current;

		SortedVectorPool<T>::EntityEntry new_entry;
		new_entry.Entity = entity.Handle;

		m_components.Insert( std::move( new_entry ), current );

		return &m_components[current].Component;
	}

	template <typename T>
	T* SortedVectorPool<T>::Find( const EntityHandle& entity ) const
	{
		int min = 0;
		int max = (int) m_components.Size();

		// empty vector
		if( max == 0 )
			return nullptr;

		while( max >= min )
		{
			int mid = (min + max) / 2;
			EntityEntry& entry = m_components[mid];

			if( entry.Entity == entity.Handle )
			{
				return &entry.Component;
			}

			if( entry.Entity < entity.Handle )
			{
				min = mid + 1;
			}
			else if( entry.Entity >= entity.Handle )
			{
				max = mid - 1;
			}
		}

		return nullptr;
	}

	template <typename T>
	void SortedVectorPool<T>::Remove( const EntityHandle& entity )
	{
		int min = 0;
		int max = (int) m_components.Size();

		// empty vector
		if( max == 0 )
			return;

		int index = -1;

		while( max >= min )
		{
			index = (min + max) / 2;
			EntityEntry& entry = m_components[index];

			if( entry.Entity == entity.Handle )
			{
				break;
			}

			if( entry.Entity < entity.Handle )
			{
				min = index + 1;
			}
			else if( entry.Entity >= entity.Handle )
			{
				max = index - 1;
			}
		}

		if( index != -1 )
			m_components.RemoveOrdered( index );
	}

	template <typename T>
	bool SortedVectorPool<T>::Exists( const EntityHandle& entity ) const
	{
		T* cmp = Find( entity );

		return cmp != nullptr;
	}

	template<typename TIter, typename TValue>
	SortedVectorIterator<TIter, TValue>::SortedVectorIterator( TIter init )
	{
		m_current = init;
	}

	template<typename TIter, typename TValue>
	SortedVectorIterator<TIter, TValue>::SortedVectorIterator( const SortedVectorIterator<TIter, TValue>& other )
	{
		m_current = other.m_current;
	}

	template<typename TIter, typename TValue>
	TValue& SortedVectorIterator<TIter, TValue>::operator*() const
	{
		return m_current->Component;
	}

	template<typename TIter, typename TValue>
	bool SortedVectorIterator<TIter, TValue>::operator==( const SortedVectorIterator<TIter, TValue>& other )
	{
		return m_current == other.m_current;
	}

	template<typename TIter, typename TValue>
	bool SortedVectorIterator<TIter, TValue>::operator!=( const SortedVectorIterator<TIter, TValue>& other )
	{
		return !operator==( other );
	}

	template<typename TIter, typename TValue>
	SortedVectorIterator<TIter, TValue>& SortedVectorIterator<TIter, TValue>::operator++()
	{
		++m_current;

		return *this;
	}

	template <typename T>
	typename SortedVectorPool<T>::iterator SortedVectorPool<T>::begin() const
	{
		return iterator( m_components.begin() );
	}

	template <typename T>
	typename SortedVectorPool<T>::iterator SortedVectorPool<T>::end() const
	{
		return iterator( m_components.end() );
	}
}