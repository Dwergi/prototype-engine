#pragma once

namespace ddc
{
	struct ScratchEntity
	{
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

		template <typename TComponent> TComponent* Access() const;
		template <typename TComponent> const TComponent* Get() const;
		template <typename TComponent> bool Has() const;

	private:

		struct ComponentEntry
		{
			dd::ComponentID Type { 0 };
			size_t Offset { 0 };
			size_t Size { 0 };
			uint64 Hash { 0 }; // hash of storage prior to modification
		};

		ddc::Entity m_entity;
		dd::Array<ComponentEntry, MAX_COMPONENTS> m_components;
		TagBits m_tags;
		byte* m_storage { nullptr };

		void* FindComponent(dd::ComponentID id) const;
	};

	template <typename TComponent>
	TComponent* ScratchEntity::Access() const
	{
		dd::ComponentID id = DD_FIND_TYPE(TComponent)->ComponentID();
		return (TComponent*) FindComponent(id);
	}

	template <typename TComponent>
	const TComponent* ScratchEntity::Get() const
	{
		dd::ComponentID id = DD_FIND_TYPE(TComponent)->ComponentID();
		return (const TComponent*) FindComponent(id);
	}

	template <typename TComponent>
	bool ScratchEntity::Has() const
	{
		dd::ComponentID id = DD_FIND_TYPE(TComponent)->ComponentID();
		void* data = FindComponent(id);
		return data != nullptr;
	}
}