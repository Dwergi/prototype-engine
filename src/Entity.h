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

		EntitySpace* Space() const;

		template <typename TComponent> TComponent* Access() const;
		template <typename TComponent> const TComponent* Get() const;
		template <typename TComponent> bool Has() const;
		template <typename TComponent> TComponent& Add() const;
		template <typename TComponent> void Remove() const;

		union
		{
			struct
			{
				uint64 ID : 32;
				uint64 Version : 28;
				uint64 m_space : 4;
			};

			uint64 Handle { ~0u };
		};

	private:
		friend struct EntitySpace;
	};

	// ASSUMPTION: Entity is included from EntitySpace.h, which is always included.
	// All methods are defined in EntitySpace.h/cpp.
}