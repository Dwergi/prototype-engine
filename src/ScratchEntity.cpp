#include "PCH.h"
#include "ScratchEntity.h"

namespace ddc
{
	ScratchEntity::ScratchEntity(ddc::Entity entity)
	{
		DD_ASSERT(entity.IsValid());
		DD_ASSERT(entity.IsAlive());
		
		EntitySpace* space = entity.Space();
		size_t total_size = 0;

		dd::Array<const dd::TypeInfo*, MAX_COMPONENTS> cmp_types;
		for (dd::ComponentID id = 0; id < dd::TypeInfo::ComponentCount(); ++id)
		{
			if (space->HasComponent(entity, id))
			{
				const dd::TypeInfo* type_info = dd::TypeInfo::GetComponent(id);
				cmp_types.Add(type_info);

				total_size += type_info->Size();
			}
		}

		m_storage = new byte[total_size];

		size_t current_offset = 0;
		for (const dd::TypeInfo* type : cmp_types)
		{
			dd::ComponentID id = type->ComponentID();
			size_t size = type->Size();

			const byte* data = (const byte*)space->GetComponent(entity, id);
			memcpy(m_storage + current_offset, data, size);
			current_offset += size;

			ComponentEntry entry;
			entry.Type = id;
			entry.Offset = current_offset;
			entry.Hash = dd::HashBytes(data, size);
			entry.Size = size;
			m_components.Add(entry);
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

		for (const ComponentEntry& entry : m_components)
		{
			const byte* data = m_storage + entry.Offset;
			uint64 new_hash = dd::HashBytes(data, entry.Size);

			if (new_hash != entry.Hash)
			{
				void* cmp_data = space->AccessComponent(m_entity, entry.Type);
				memcpy(cmp_data, data, entry.Size);

				changed = true;
			}
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
		for (const ComponentEntry& entry : m_components)
		{
			if (entry.Type == id)
			{
				return m_storage + entry.Offset;
			}
		}
		return nullptr;
	}
}