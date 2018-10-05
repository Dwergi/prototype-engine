//
// EntityPrototype.h - A prototype for an entity from which new entities can be spawned.
// Copyright (C) Sebastian Nordgren 
// October 5th 2018
//

#pragma once

#include "World.h"

namespace ddc
{
	struct World;

	struct ComponentPrototype
	{
		dd::ComponentID ID { dd::INVALID_COMPONENT };
		void* Data { nullptr };
		size_t Size { 0 };

		template <typename TComponent>
		ComponentPrototype( const TComponent& cmp );
		ComponentPrototype( const void* data, dd::ComponentID id );
		ComponentPrototype( ComponentPrototype&& other );
		~ComponentPrototype();

		void CopyTo( void* cmp ) const;

	private:
		void Initialize( const void* data, const dd::TypeInfo* typeInfo );
	};

	struct EntityPrototype
	{
		EntityPrototype( std::string name );

		template <typename TComponent>
		void AddComponent( const TComponent& cmp );

		void AddTag( ddc::Tag tag );

		void CreateFromEntity( ddc::Entity entity, const ddc::World& world );
		ddc::Entity Instantiate( ddc::World& world );

		std::string Name;
		TagBits Tags;
		std::vector<ComponentPrototype> Components;
	};

	template <typename TComponent>
	ComponentPrototype::ComponentPrototype( const TComponent& cmp )
	{
		const TypeInfo* typeInfo = DD_TYPE( TComponent );
		DD_ASSERT( typeInfo != nullptr && typeInfo->ComponentID() != INVALID_COMPONENT );

		Initialize( &cmp, typeInfo );
	}

	template <typename TComponent>
	void EntityPrototype::AddComponent( const TComponent& cmp )
	{
		Components.emplace_back( cmp );
	}
}