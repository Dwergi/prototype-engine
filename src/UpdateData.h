//
// UpdateData.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "UpdateBuffer.h"

namespace ddc
{
	struct EntityLayer;
	struct ScratchEntity;
	struct System;
	struct UpdateBufferView;

	//
	// UpdateData is just a conglomeration of things that a system update may want to know about.
	// It also includes all the data views that the system has requested.
	//
	struct UpdateData
	{
		UpdateData(ddc::EntityLayer& layer, float delta_t) : m_layer(&layer), m_delta(delta_t) {}
		UpdateData(UpdateData&& other) noexcept : m_layer(other.m_layer), m_delta(other.m_delta), m_views(std::move(other.m_views)) {}

		float Delta() const { return m_delta; }
		const UpdateBufferView& Data(const char* name = nullptr) const;

		void CreateEntity(ddc::ScratchEntity&& new_entity);
		void DestroyEntity(ddc::Entity entity);

	private:
		friend struct System;
		
		ddc::EntityLayer* m_layer { nullptr };
		float m_delta { 0 };
		dd::Array<UpdateBufferView, 8> m_views;

		std::vector<ddc::ScratchEntity*> m_createdEntities;
		std::vector<ddc::Entity> m_destroyedEntities;

		const UpdateBufferView& CreateView(UpdateBuffer& buffer, size_t start, size_t count);
		void CommitChanges();
	};
}
