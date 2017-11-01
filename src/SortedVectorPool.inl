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
	class SortedVectorPoolIterator
	{
	private:

		typename Vector<typename SortedVectorPool<T>::EntityEntry>::iterator Iter;

	public:

		SortedVectorPoolIterator( typename Vector<typename SortedVectorPool<T>::EntityEntry>::iterator& it ) :
			Iter( it )
		{
		}

		SortedVectorPoolIterator( const SortedVectorPoolIterator<T>& other ) :
			Iter( other.Iter )
		{
		}

		const T& operator*() const
		{
			return Iter->Component;
		}

		T& operator*()
		{
			return Iter->Component;
		}

		SortedVectorPoolIterator<T> operator++()
		{
			++Iter;

			return *this;
		}

		bool operator!=( const SortedVectorPoolIterator<T>& other ) const
		{
			return Iter != other.Iter;
		}
	};

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
	int SortedVectorPool<T>::Size() const
	{
		return m_components.Size();
	}

	template <typename T>
	T* SortedVectorPool<T>::Create( EntityHandle entity )
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

			int current_handle = m_components[current].Entity;

			if( current_handle == entity.Handle )
			{
				// already exists
				DD_ASSERT( false );
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

		T* cmp = &m_components[current].Component;

		ComponentBase* baseptr = static_cast<ComponentBase*>(cmp);
		baseptr->Entity = entity;

		return cmp;
	}

	template <typename T>
	T* SortedVectorPool<T>::Find( EntityHandle entity ) const
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
	void SortedVectorPool<T>::Remove( EntityHandle entity )
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
	bool SortedVectorPool<T>::Exists( EntityHandle entity ) const
	{
		T* cmp = Find( entity );

		return cmp != nullptr;
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