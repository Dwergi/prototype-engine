#pragma once

namespace ddc
{
	enum class Tag : uint
	{
		None = 0,
		Visible = 1,
		Focused = 2,
		Selected = 3,
		Static = 4,
		Dynamic = 5
	};

	struct EntitySpace;

	struct Entity
	{
		bool operator==(Entity other) const { return Handle == other.Handle && m_space == other.m_space; }

		bool IsValid() const;
		bool IsAlive() const;
		int Components() const;

		void AddTag(ddc::Tag tag) const;
		void RemoveTag(ddc::Tag tag) const;
		bool HasTag(ddc::Tag tag) const;

		EntitySpace* Space() const { return m_space; }

		template <typename TComponent> TComponent* Access() const;
		template <typename TComponent> const TComponent* Get() const;
		template <typename TComponent> bool Has() const;
		template <typename TComponent> TComponent& Add() const;
		template <typename TComponent> void Remove() const;

		union
		{
			struct
			{
				uint ID : 22;
				uint Version : 10;
			};

			uint Handle { ~0u };
		};

	private:
		friend struct EntitySpace;
		EntitySpace* m_space { nullptr };
	};

	// ASSUMPTION: Entity is included from EntitySpace.h, which is always included.
	// All methods are defined in EntitySpace.h.
}