//
// ComponentHandle.h - A handle to a component.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	class EntityHandle;

	//
	// A component handle - this can be both readable and writable, but you must be explicit in how you're going to use it.
	//
	template <typename TComponent>
	class ComponentHandle
	{
	public:

		ComponentHandle()
		{
		}

		ComponentHandle( EntityHandle entity )
			: m_entity( entity )
		{
		}

		ComponentHandle( const ComponentHandle& other )
			: m_entity( other.m_entity )
		{
		}

		bool IsValid() const
		{
			// valid if entity is valid
			return m_entity.IsValid() && m_entity.Has<TComponent>();
		}

		const TComponent* Read() const 
		{
			if( !IsValid() )
			{
				DD_ASSERT( false, "Handle not valid!" );
				return nullptr;
			}

			return m_entity.m_manager->GetReadable<TComponent>( m_entity );
		}

		TComponent* Write() const
		{
			if( !IsValid() )
			{
				DD_ASSERT( false, "Handle not valid!" );
				return nullptr;
			}

			return m_entity.m_manager->GetWritable<TComponent>( m_entity );
		}

	private:

		EntityHandle m_entity;
	};
}