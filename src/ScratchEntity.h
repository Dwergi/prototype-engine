#pragma once

namespace ddc
{
	struct ScratchEntity
	{
		ScratchEntity(ddc::Entity entity);
		ScratchEntity(const ScratchEntity& other);
		~ScratchEntity();

		int Components() const;

		bool IsValid() const;
		bool IsAlive() const;

		void AddTag(ddc::Tag tag) const;
		void RemoveTag(ddc::Tag tag) const;
		bool HasTag(ddc::Tag tag) const;

		template <typename TComponent> TComponent* Access() const;
		template <typename TComponent> const TComponent* Get() const;
		template <typename TComponent> bool Has() const;
		template <typename TComponent> TComponent& Add() const;
		template <typename TComponent> void Remove() const;

	private:

		struct ComponentEntry
		{
			ComponentID Type;
			size_t Offset;
		};

		Array<ComponentEntry, MAX_COMPONENTS> m_offsets;
		byte* m_storage;

		// hash of storage prior to modification
		size_t m_hash; 
	};
}