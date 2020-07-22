#pragma once

namespace ddc
{
	struct ScratchEntity
	{
		ScratchEntity();
		ScratchEntity(ddc::Entity entity);
		~ScratchEntity();

		// could support move, but not sure it's necessary...
		ScratchEntity(const ScratchEntity&) = delete;

		// Commit changes to the entity.
		// If this goes out of scope without Commit() being called, all changes are discarded.
		bool Commit();

		int Components() const { return m_components.Size(); }

		void AddTag(ddc::Tag tag);
		void RemoveTag(ddc::Tag tag);
		bool HasTag(ddc::Tag tag) const;
		TagBits GetAllTags() const;

		template <typename TComponent> TComponent& Add();
		template <typename TComponent> void Remove();

		template <typename TComponent> TComponent* Access() const;
		template <typename TComponent> const TComponent* Get() const;
		template <typename TComponent> bool Has() const;

		void* AccessComponent(dd::ComponentID id) const;
		const void* GetComponent(dd::ComponentID id) const;
		bool HasComponent(dd::ComponentID id) const;
		void GetAllComponents(dd::IArray<dd::ComponentID>& components) const;

	private:

		struct ComponentEntry
		{
			const dd::TypeInfo* Type { nullptr };
			uint64 Hash { 0 }; // hash of storage prior to modification
			bool Deleted { false };
		};

		ddc::Entity m_entity;
		dd::Array<ComponentEntry, MAX_COMPONENTS> m_components;
		TagBits m_tags;
		dd::Buffer<byte> m_storage;
	};

	template <typename TComponent>
	TComponent* ScratchEntity::Access() const
	{
		dd::ComponentID id = DD_FIND_TYPE(TComponent)->ComponentID();
		return (TComponent*) AccessComponent(id);
	}

	template <typename TComponent>
	const TComponent* ScratchEntity::Get() const
	{
		dd::ComponentID id = DD_FIND_TYPE(TComponent)->ComponentID();
		return (const TComponent*) GetComponent(id);
	}

	template <typename TComponent>
	bool ScratchEntity::Has() const
	{
		dd::ComponentID id = DD_FIND_TYPE(TComponent)->ComponentID();
		void* data = HasComponent(id);
		return data != nullptr;
	}

	template <typename TComponent>
	TComponent& ScratchEntity::Add()
	{
		const dd::TypeInfo* type = DD_FIND_TYPE(TComponent);
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

		TComponent* new_cmp = new (&m_storage[old_size]) TComponent();
		return *new_cmp;
	}

	template <typename TComponent> 
	void ScratchEntity::Remove()
	{
		const dd::TypeInfo* type = DD_FIND_TYPE(TComponent);
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