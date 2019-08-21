//
// EntityPrototype.h - A prototype for an entity from which new entities can be spawned.
// Copyright (C) Sebastian Nordgren 
// October 5th 2018
//

#pragma once

#include "HandleManager.h"

namespace ddc
{
	struct EntityLayer;

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

	struct EntityPrototype : dd::HandleTarget
	{
		template <typename TComponent>
		void AddComponent( const TComponent& cmp );

		void AddTag( ddc::Tag tag );

		void PopulateFromEntity( ddc::Entity entity, const ddc::EntityLayer& entities );
		ddc::Entity Instantiate( ddc::EntityLayer& entities );

		TagBits Tags;
		std::vector<ComponentPrototype> Components;
	};

	template <typename TComponent>
	ComponentPrototype::ComponentPrototype( const TComponent& cmp )
	{
		const TypeInfo* typeInfo = DD_FIND_TYPE( TComponent );
		DD_ASSERT( typeInfo != nullptr && typeInfo->ComponentID() != INVALID_COMPONENT );

		Initialize( &cmp, typeInfo );
	}

	template <typename TComponent>
	void EntityPrototype::AddComponent( const TComponent& cmp )
	{
		Components.emplace_back( cmp );
	}

	using EntityPrototypeHandle = dd::Handle<EntityPrototype>;

	using EntityPrototypeManager = dd::HandleManager<EntityPrototype>;
}