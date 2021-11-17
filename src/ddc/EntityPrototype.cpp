//
// EntityPrototype.cpp - A prototype for an entity from which new entities can be spawned.
// Copyright (C) Sebastian Nordgren 
// October 5th 2018
//

#include "PCH.h"
#include "EntityPrototype.h"

namespace ddc
{
	ComponentPrototype::ComponentPrototype(const void* data, dd::ComponentID id)
	{
		const dd::TypeInfo* typeInfo = dd::TypeInfo::GetComponent(id);
		DD_ASSERT(typeInfo != nullptr);

		Initialize(data, typeInfo);
	}

	ComponentPrototype::ComponentPrototype(ComponentPrototype&& other) noexcept
	{
		std::swap(Data, other.Data);
		std::swap(Size, other.Size);
		std::swap(ID, other.ID);
	}

	ComponentPrototype::~ComponentPrototype()
	{
		if (Data != nullptr)
		{
			delete[] Data;
		}
	}

	void ComponentPrototype::Initialize(const void* data, const dd::TypeInfo* typeInfo)
	{
		ID = typeInfo->ComponentID();
		Size = typeInfo->Size();
		Data = new byte[Size];

		memcpy(Data, data, Size);
	}

	void ComponentPrototype::CopyTo(void* cmp) const
	{
		DD_ASSERT(Data != nullptr && Size > 0);

		memcpy(cmp, Data, Size);
	}

	void EntityPrototype::PopulateFromEntity(ddc::Entity entity)
	{
		DD_ASSERT(Components.empty(), "Already initialized EntityPrototype!");

		Tags = entity.GetAllTags();

		dd::Array<dd::ComponentID, MAX_COMPONENTS> components;
		entity.GetAllComponents(components);

		for (dd::ComponentID id : components)
		{
			const void* cmp = entity.GetComponent(id);
			Components.emplace_back(cmp, id);
		}
	}

	void EntityPrototype::PopulateFromScratch(const ddc::ScratchEntity& entity)
	{
		DD_ASSERT(Components.empty(), "Already initialized EntityPrototype!");

		Tags = entity.GetAllTags();

		dd::Array<dd::ComponentID, MAX_COMPONENTS> components;
		entity.GetAllComponents(components);

		for (dd::ComponentID id : components)
		{
			const void* cmp = entity.GetComponent(id);
			Components.emplace_back(cmp, id);
		}
	}

	ddc::Entity EntityPrototype::Instantiate(ddc::EntityLayer& layer) const
	{
		ddc::Entity entity = layer.CreateEntity();

		for (const ComponentPrototype& cmp : Components)
		{
			void* data = layer.AddComponent(entity, cmp.ID);
			cmp.CopyTo(data);
		}

		layer.SetAllTags(entity, Tags);

		return entity;
	}

	ddc::ScratchEntity EntityPrototype::InstantiateScratch() const
	{
		ddc::ScratchEntity scratch;

		for (const ComponentPrototype& cmp : Components)
		{
			void* data = scratch.AddComponent(cmp.ID);
			cmp.CopyTo(data);
		}

		scratch.SetAllTags(Tags);

		return scratch;
	}

	void EntityPrototype::AddTag(ddc::Tag tag)
	{
		Tags.Set(tag);
	}
}