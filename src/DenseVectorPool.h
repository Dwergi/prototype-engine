#pragma once

#include "Assert.h"
#include "ComponentPoolBase.h"

//
// A dense component pool is one for which the component is assumed to exist for all entities (eg. transform).
// Uses much more space than a sparse component pool for large numbers of entities.
//
template<typename T>
class DenseVectorPool : public ComponentPoolBase
{
public:

	static_assert( std::is_base_of<Component, T>::value, "Not derived from Component." );

#ifdef USE_EIGEN
	typedef typename aligned_vector<T> Storage;
#else
	typedef typename std::vector<T> Storage;
#endif

	typedef typename Storage::iterator ComponentIterator;

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
	T* Create( const EntityHandle& entity )
	{
		// already allocated!
		if( Exists( entity ) )
		{
			ASSERT( false );
			return nullptr;
		}

		if( entity.ID >= (int) m_components.size() )
		{
			m_components.resize( entity.ID + 1 );
			m_valid.resize( entity.ID + 1 );
		}

		m_valid[ entity.ID ] = true;

		T& cmp = m_components[ entity.ID ];
		return &cmp;
	}

	//
	// Find the component for the given entity.
	// Returns null if the component hasn't been created.
	// 
	T* Find( const EntityHandle& entity )
	{
		if( !Exists( entity ) )
		{
			ASSERT( false );
			return nullptr;
		}

		T& cmp = m_components[ entity.ID ];
		return &cmp;
	}

	//
	// Remove the component associated with the given entity.
	// 
	void Remove( const EntityHandle& entity )
	{
		if( entity.ID >= (int) m_valid.size() )
		{
			ASSERT( false );
			return;
		}

		m_valid[ entity.ID ] = false;
	}

	// 
	// Checks if the given entity has a component of this type.
	// 
	bool Exists( const EntityHandle& entity )
	{
		if( entity.ID >= (int) m_valid.size() )
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

private:

	Storage m_components;
	std::vector<bool> m_valid;
};