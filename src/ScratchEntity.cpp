#include "PCH.h"
#include "ScratchEntity.h"

DD_OPTIMIZE_OFF();

namespace ddc
{
	ScratchEntity::ScratchEntity()
	{

	}

	ScratchEntity::ScratchEntity(ScratchEntity&& other) noexcept
	{
		std::swap(m_entity, other.m_entity);
		std::swap(m_components, other.m_components);
		std::swap(m_tags, other.m_tags);
		std::swap(m_storage, other.m_storage);
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

	void ScratchEntity::SetAllTags(ddc::TagBits tags)
	{
		m_tags = tags;
	}

	ScratchEntity::~ScratchEntity()
	{
		byte* storage = m_storage.Release();
		delete[] storage;
	}

	bool ScratchEntity::Commit()
	{
		DD_ASSERT(m_entity.IsValid() && m_entity.IsAliveOrCreated(), "Committing to an an invalid or dead entity!");

		EntityLayer* layer = m_entity.Layer();

		bool changed = false;

		size_t current_offset = 0;
		for (const ComponentEntry& entry : m_components)
		{
			const byte* data = &m_storage[current_offset];
			size_t size = entry.Type->Size();

			if (!layer->HasComponent(m_entity, entry.Type->ComponentID()))
			{
				// component added
				void* cmp_data = layer->AddComponent(m_entity, entry.Type->ComponentID());
				memcpy(cmp_data, data, size);

				changed = true;
			}
			else if (entry.Deleted)
			{
				// component deleted
				layer->RemoveComponent(m_entity, entry.Type->ComponentID());

				changed = true;
			}
			else if (dd::HashBytes(data, size) != entry.Hash)
			{
				// component changed
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

	ddc::Entity ScratchEntity::Instantiate(ddc::EntityLayer& layer)
	{
		m_entity = layer.CreateEntity();
		Commit();
		return m_entity;
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

	void* ScratchEntity::AddComponent(dd::ComponentID id)
	{
		const dd::TypeInfo* type = dd::TypeInfo::GetComponent(id);
		return AddComponentType(type);
	}

	void* ScratchEntity::AddComponentType(const dd::TypeInfo* type)
	{
		DD_ASSERT(!HasComponent(type->ComponentID()));

		ComponentEntry new_entry;
		new_entry.Type = type;
		m_components.Add(new_entry);

		size_t old_size = m_storage.SizeBytes();
		size_t new_size = old_size + type->Size();

		byte* old_ptr = m_storage.Release();
		m_storage.Set(new byte[new_size], new_size);

		memcpy(m_storage.Access(), old_ptr, old_size);
		delete[] old_ptr;

		type->PlacementNew(&m_storage[old_size]);

		return &m_storage[old_size];
	}

	void ScratchEntity::RemoveComponent(dd::ComponentID id)
	{
		const dd::TypeInfo* type = dd::TypeInfo::GetComponent(id);
		RemoveComponentType(type);
	}

	void ScratchEntity::RemoveComponentType(const dd::TypeInfo* type)
	{
		DD_ASSERT(HasComponent(type->ComponentID()));

		for (ComponentEntry& entry : m_components)
		{
			if (entry.Type == type)
			{
				entry.Deleted = true;
				break;
			}
		}
	}
}