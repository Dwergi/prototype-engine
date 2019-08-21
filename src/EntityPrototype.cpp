//
// EntityPrototype.cpp - A prototype for an entity from which new entities can be spawned.
// Copyright (C) Sebastian Nordgren 
// October 5th 2018
//

#include "PCH.h"
#include "EntityPrototype.h"

namespace ddc
{
	ComponentPrototype::ComponentPrototype( const void* data, dd::ComponentID id )
	{
		const dd::TypeInfo* typeInfo = dd::TypeInfo::GetComponent( id );
		DD_ASSERT( typeInfo != nullptr );

		Initialize( data, typeInfo );
	}

	ComponentPrototype::ComponentPrototype( ComponentPrototype&& other )
	{
		std::swap( Data, other.Data );
		std::swap( Size, other.Size );
		std::swap( ID, other.ID );
	}

	ComponentPrototype::~ComponentPrototype()
	{
		if( Data != nullptr )
		{
			delete[] Data;
		}
	}

	void ComponentPrototype::Initialize( const void* data, const dd::TypeInfo* typeInfo )
	{
		ID = typeInfo->ComponentID();
		Size = typeInfo->Size();
		Data = new byte[ Size ];

		memcpy( Data, data, Size );
	}

	void ComponentPrototype::CopyTo( void* cmp ) const
	{
		DD_ASSERT( Data != nullptr && Size > 0 );

		memcpy( cmp, Data, Size );
	}

	void EntityPrototype::PopulateFromEntity( ddc::Entity entity, const ddc::EntityLayer& entities )
	{
		DD_ASSERT( Components.empty(), "Already initialized EntityPrototype!" );

		Tags = entities.GetAllTags( entity );

		dd::Array<dd::ComponentID, MAX_COMPONENTS> components;
		entities.GetAllComponents( entity, components );

		for( dd::ComponentID id : components )
		{
			const void* cmp = entities.GetComponent( entity, id );
			Components.emplace_back( cmp, id );
		}
	}

	ddc::Entity EntityPrototype::Instantiate( ddc::EntityLayer& entities )
	{
		ddc::Entity entity = entities.CreateEntity();

		for( const ComponentPrototype& cmp : Components )
		{
			void* data = entities.AddComponent( entity, cmp.ID );
			cmp.CopyTo( data );
		}

		entities.SetAllTags( entity, Tags );

		return entity;
	}

	void EntityPrototype::AddTag( ddc::Tag tag )
	{
		Tags.set( (int) tag, true );
	}
}