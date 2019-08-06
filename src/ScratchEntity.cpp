#include "PCH.h"
#include "ScratchEntity.h"

namespace ddc
{
	ScratchEntity::ScratchEntity(ddc::Entity entity)
	{
		DD_ASSERT(entity.IsValid() && entity.IsAlive());

		m_entity = entity;
		
		EntitySpace* space = entity.Space();
		size_t total_size = 0;

		for (dd::ComponentID id = 0; id < dd::TypeInfo::ComponentCount(); ++id)
		{
			if (space->HasComponent(entity, id))
			{
				ComponentEntry entry;
				entry.Type = dd::TypeInfo::GetComponent(id);
				m_components.Add(entry);

				total_size += entry.Type->Size();
			}
		}

		m_storage = new byte[total_size];
		memset(m_storage, 0xCC, total_size);

		size_t current_offset = 0;
		for (ComponentEntry& entry : m_components)
		{
			dd::ComponentID id = entry.Type->ComponentID();
			size_t size = entry.Type->Size();

			const byte* data = (const byte*)space->GetComponent(entity, id);
			memcpy(m_storage + current_offset, data, size);
			
			entry.Hash = dd::HashBytes(data, size);

			current_offset += size;
		}

		m_tags = space->GetAllTags(entity);
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

	ScratchEntity::~ScratchEntity()
	{
		delete[] m_storage;
	}

	bool ScratchEntity::Commit()
	{
		EntitySpace* space = m_entity.Space();

		bool changed = false;

		size_t current_offset = 0;
		for (const ComponentEntry& entry : m_components)
		{
			const byte* data = m_storage + current_offset;
			size_t size = entry.Type->Size();

			uint64 new_hash = dd::HashBytes(data, size);
			if (new_hash != entry.Hash)
			{
				void* cmp_data = space->AccessComponent(m_entity, entry.Type->ComponentID());
				memcpy(cmp_data, data, size);

				changed = true;
			}

			current_offset += size;
		}

		if (m_tags != space->GetAllTags(m_entity))
		{
			space->SetAllTags(m_entity, m_tags);
			changed = true;
		}

		return changed;
	}

	void* ScratchEntity::FindComponent(dd::ComponentID id) const
	{
		size_t current_offset = 0;
		for (const ComponentEntry& entry : m_components)
		{
			if (entry.Type->ComponentID() == id)
			{
				return m_storage + current_offset;
			}

			current_offset += entry.Type->Size();
		}
		return nullptr;
	}
}