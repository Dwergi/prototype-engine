#pragma once

namespace ddc
{
	struct ScratchEntity
	{
		ScratchEntity();
		ScratchEntity(ddc::Entity entity);
		ScratchEntity(ScratchEntity&& other) noexcept;
		~ScratchEntity();

		// Commit changes to the entity.
		// If this goes out of scope without Commit() being called, all changes are discarded.
		bool Commit();

		// Create a new instance of this entity in the given layer.
		ddc::Entity Instantiate(ddc::EntityLayer& layer);

		int Components() const { return m_components.Size(); }

		void AddTag(ddc::Tag tag);
		void RemoveTag(ddc::Tag tag);
		bool HasTag(ddc::Tag tag) const;
		dd::EnumFlags<ddc::Tag> GetAllTags() const;
		void SetAllTags(dd::EnumFlags<ddc::Tag> tags);

		template <typename TComponent> TComponent& Add();
		template <typename TComponent> void Remove();

		template <typename TComponent> TComponent* Access() const;
		template <typename TComponent> const TComponent* Get() const;
		template <typename TComponent> bool Has() const;

		void* AddComponent(dd::ComponentID id);
		void RemoveComponent(dd::ComponentID id);

		void* AccessComponent(dd::ComponentID id) const;
		const void* GetComponent(dd::ComponentID id) const;
		bool HasComponent(dd::ComponentID id) const;
		void GetAllComponents(dd::IArray<dd::ComponentID>& components) const;

		template <typename... TComponents>
		static ScratchEntity Create();

	private:

		struct ComponentEntry
		{
			const dd::TypeInfo* Type { nullptr };
			uint64 Hash { 0 }; // hash of storage prior to modification
			bool Deleted { false };
		};

		ddc::Entity m_entity;
		dd::Array<ComponentEntry, MAX_COMPONENTS> m_components;
		dd::EnumFlags<ddc::Tag> m_tags;
		dd::Buffer<byte> m_storage;

		void* AddComponentType(const dd::TypeInfo* type);
		void RemoveComponentType(const dd::TypeInfo* type);
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
		return *static_cast<TComponent*>(AddComponentType(type));
	}

	template <typename TComponent> 
	void ScratchEntity::Remove()
	{
		const dd::TypeInfo* type = DD_FIND_TYPE(TComponent);
		RemoveComponentType(type);
	}

	template <typename... TComponents>
	ScratchEntity ScratchEntity::Create()
	{
		ScratchEntity scratch;

		ExpandType
		{
			0, (scratch.Add<TComponents>(), 0)...
		};

		return std::move(scratch);
	}
}