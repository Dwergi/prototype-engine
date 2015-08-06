//
// DenseVectorPool.h - A pool that holds components in a vector.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Assert.h"
#include "ComponentPoolBase.h"
#include "ComponentHandle.h"

namespace dd
{
	//
	// A dense component pool is one for which the component is assumed to exist for all entities (eg. transform).
	// Uses much more space than a sparse component pool for large numbers of entities.
	//
	template<typename T>
	class DenseVectorPool : public dd::ComponentPoolBase
	{
	public:
		static_assert( std::is_base_of<dd::Component, T>::value, "Not derived from Component." );

		typedef typename std::vector<T> Storage;
		typedef typename Storage::iterator ComponentIterator;
		typedef typename Storage::const_iterator ComponentConstIterator;

		DenseVectorPool()
		{

		}

		~DenseVectorPool()
		{
			Cleanup();
		}

		void Cleanup()
		{
			m_components.swap( Storage() );
			m_valid.swap( std::vector<bool>() );
		}

		//
		// Checks if this component pool is empty or not.
		// 
		bool Empty()
		{
			bool any = false;
			for( bool valid : m_valid )
			{
				any |= valid;
			}

			return any;
		}

		//
		// Create a new component of this type for the given entity and return the pointer to it.
		// Returns null if the component already exists.
		// 
		T* Create( const dd::EntityHandle& entity )
		{
			// already allocated!
			if( Exists( entity ) )
			{
				ASSERT( false, "Entity already exists!" );
				return nullptr;
			}

			if( entity.ID < 0 || entity.ID >= (int) m_components.size() )
			{
				m_components.resize( entity.ID + 1 );
				m_valid.resize( entity.ID + 1 );
			}

			m_valid[ entity.ID ] = true;

			T& cmp = m_components[ entity.ID ];
			return &cmp;
		}

		dd::ComponentHandle<T> GetHandle( const dd::EntityHandle& entity )
		{
			return dd::ComponentHandle<T>( entity, *this );
		}

		//
		// Find the component for the given entity.
		// Returns null if the component hasn't been created.
		// 
		T* Find( const dd::EntityHandle& entity )
		{
			if( !Exists( entity ) )
			{
				ASSERT( false, "Entity does not exist!" );
				return nullptr;
			}

			T& cmp = m_components[ entity.ID ];
			return &cmp;
		}

		//
		// Remove the component associated with the given entity.
		// 
		void Remove( const dd::EntityHandle& entity )
		{
			if( entity.ID < 0 || entity.ID >= (int) m_valid.size() )
			{
				ASSERT( false, "Entity ID outside of valid range!" );
				return;
			}

			m_valid[ entity.ID ] = false;
		}

		// 
		// Checks if the given entity has a component of this type.
		// 
		bool Exists( const dd::EntityHandle& entity )
		{
			if( entity.ID < 0 || entity.ID >= (int) m_valid.size() )
				return false;

			bool isValid = m_valid[ entity.ID ];
			return isValid;
		}

		ComponentIterator begin()
		{
			return m_components.begin();
		}

		ComponentIterator end()
		{
			return m_components.end();
		}

		ComponentConstIterator begin() const
		{
			return m_components.begin();
		}

		ComponentConstIterator end() const
		{
			return m_components.end();
		}

		DenseVectorPool& operator=( const DenseVectorPool<T>& other )
		{
			m_components = other.m_components;
			m_valid = other.m_valid;

			return *this;
		}

	private:

		Storage m_components;
		std::vector<bool> m_valid;
	};
}