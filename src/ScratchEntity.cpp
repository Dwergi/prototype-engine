#include "PCH.h"
#include "ScratchEntity.h"

namespace ddc
{
	ScratchEntity::ScratchEntity()
	{

	}

	ScratchEntity::ScratchEntity(ddc::Entity entity)
	{
		DD_ASSERT(entity.IsValid() && entity.IsAlive());

		m_entity = entity;
		
		EntityLayer* layer = entity.Layer();
		size_t total_size = 0;

		for (dd::ComponentID id = 0; id < dd::TypeInfo::ComponentCount(); ++id)
		{
			if (layer->HasComponent(entity, id))
			{
				ComponentEntry entry;
				entry.Type = dd::TypeInfo::GetComponent(id);
				m_components.Add(entry);

				total_size += entry.Type->Size();
			}
		}

		m_storage = dd::Buffer<byte>(new byte[total_size], total_size);
		memset(m_storage.Access(), 0xCC, total_size);

		size_t current_offset = 0;
		for (ComponentEntry& entry : m_components)
		{
			dd::ComponentID id = entry.Type->ComponentID();
			size_t size = entry.Type->Size();

			const byte* data = (const byte*) layer->GetComponent(entity, id);
			memcpy(&m_storage[current_offset], data, size);
			
			entry.Hash = dd::HashBytes(data, size);

			current_offset += size;
		}

		m_tags = layer->GetAllTags(entity);
	}

	void ScratchEntity::AddTag(ddc::Tag tag)
	{
		m_tags.set((size_t) tag);
	}

	void ScratchEntity::RemoveTag(ddc::Tag tag)
	{
		m_tags.reset((size_t) tag);
	}

	bool ScratchEntity::HasTag(ddc::Tag tag) const
	{
		return m_tags.test((size_t) tag);
	}

	TagBits ScratchEntity::GetAllTags() const
	{
		return m_tags;
	}

	ScratchEntity::~ScratchEntity()
	{
		byte* storage = m_storage.Release();
		delete[] storage;
	}

	bool ScratchEntity::Commit()
	{
		DD_ASSERT(m_entity.IsValid() && m_entity.IsAlive(), "Committing to an an invalid or dead entity!");

		EntityLayer* layer = m_entity.Layer();

		bool changed = false;

		size_t current_offset = 0;
		for (const ComponentEntry& entry : m_components)
		{
			const byte* data = &m_storage[current_offset];
			size_t size = entry.Type->Size();

			uint64 new_hash = dd::HashBytes(data, size);

			if (!layer->HasComponent(m_entity, entry.Type->ID()))
			{
				// component added
				void* cmp_data = layer->AddComponent(m_entity, entry.Type->ID());
				memcpy(cmp_data, data, size);

				changed = true;
			}
			else if (entry.Deleted)
			{
				layer->RemoveComponent(m_entity, entry.Type->ComponentID());

				changed = true;
			}
			else if (new_hash != entry.Hash)
			{
				void* cmp_data = layer->AccessComponent(m_entity, entry.Type->ComponentID());
				memcpy(cmp_data, data, size);

				changed = true;
			}

			current_offset += size;
		}

		if (m_tags != layer->GetAllTags(m_entity))
		{
			layer->SetAllTags(m_entity, m_tags);
			changed = true;
		}

		return changed;
	}

	void* ScratchEntity::AccessComponent(dd::ComponentID id) const
	{
		DD_ASSERT(id != dd::INVALID_COMPONENT);

		size_t current_offset = 0;
		for (const ComponentEntry& entry : m_components)
		{
			if (entry.Type->ComponentID() == id)
			{
				return &m_storage[current_offset];
			}

			current_offset += entry.Type->Size();
		}
		return nullptr;
	}

	const void* ScratchEntity::GetComponent(dd::ComponentID id) const
	{
		return AccessComponent(id);
	}

	bool ScratchEntity::HasComponent(dd::ComponentID id) const
	{
		return GetComponent(id) != nullptr;
	}

	void ScratchEntity::GetAllComponents(dd::IArray<dd::ComponentID>& components) const
	{
		for (const ComponentEntry& entry : m_components)
		{
			if (!entry.Deleted)
			{
				components.Add(entry.Type->ComponentID());
			}
		}
	}
}