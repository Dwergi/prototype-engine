//
// Entity.h
// Copyright (C) Sebastian Nordgren 
// August 21st 2019
//

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

	static const int MAX_TAGS = 32;
	typedef std::bitset<MAX_TAGS> TagBits;

	struct EntityLayer;

	struct Entity
	{
		bool operator==(Entity other) const { return Handle == other.Handle && m_layer == other.m_layer; }

		bool IsValid() const;
		bool IsAlive() const;
		bool IsAliveOrCreated() const;
		int Components() const;

		void AddTag(ddc::Tag tag) const;
		void RemoveTag(ddc::Tag tag) const;
		bool HasTag(ddc::Tag tag) const;
		TagBits GetAllTags() const;

		void Destroy() const;

		EntityLayer* Layer() const;

		void* AccessComponent(dd::ComponentID id) const;
		const void* GetComponent(dd::ComponentID id) const;
		bool HasComponent(dd::ComponentID id) const;
		void GetAllComponents(dd::IArray<dd::ComponentID>& components) const;

		template <typename TComponent> TComponent* Access() const;
		template <typename TComponent> const TComponent* Get() const;
		template <typename TComponent> bool Has() const;
		template <typename TComponent> TComponent& Add() const;
		template <typename TComponent> void Remove() const;

		static const uint64 INVALID_HANDLE = 0xFFFFFFFF;

		union
		{
			struct
			{
				uint64 ID : 32;
				uint64 Version : 28;
				uint64 m_layer : 4;
			};

			uint64 Handle { INVALID_HANDLE };
		};

		DD_BEGIN_CLASS(ddc::Entity)
			DD_MEMBER(Handle);
		DD_END_CLASS()

	private:
		friend struct EntityLayer;
	};

	// ASSUMPTION: Entity is included from EntityLayer.h, which is always included.
	// All methods are defined in EntityLayer.h/cpp.
}