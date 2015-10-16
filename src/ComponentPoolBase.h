//
// ComponentPoolBase.h - Base class for component pools.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Component.h"
#include "ComponentHandle.h"
#include "EntityHandle.h"

namespace dd
{
	class ComponentPoolBase
	{
	protected:
		ComponentPoolBase()
		{
		}
	};

	template <typename T>
	class ComponentPool
		: public ComponentPoolBase
	{
		static_assert(std::is_base_of<Component, T>::value, "Not derived from Component.");

	public:

		virtual void Clear() = 0;
		virtual uint Size() const = 0;
		virtual void Remove( const EntityHandle& ) = 0;
		virtual bool Exists( const EntityHandle& ) const = 0;
		virtual T* Create( const EntityHandle& ) = 0;
		virtual T* Find( const EntityHandle& ) const = 0;
		
		bool Empty() const
		{
			return Size() == 0;
		}

		ComponentHandle<T> GetHandle( const EntityHandle& entity ) const
		{
			return ComponentHandle<T>( entity, *this );
		}
	};
}