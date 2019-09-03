//
// UpdateData.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "UpdateBuffer.h"

namespace dd
{
	struct Job;
}

namespace ddc
{
	struct EntityLayer;
	struct System;
	struct UpdateBufferView;

	//
	// UpdateData is just a conglomeration of things that a system update may want to know about.
	// It also includes all the data views that the system has requested.
	//
	struct UpdateData
	{
		UpdateData(ddc::EntityLayer& layer, dd::Job* job, float delta_t) : m_layer(&layer), m_job(job), m_delta(delta_t) {}
		UpdateData(UpdateData&& other) : m_layer(other.m_layer), m_job(other.m_job), m_delta(other.m_delta), m_views(std::move(other.m_views)) {}

		float Delta() const { return m_delta; }
		ddc::EntityLayer& EntityLayer() const { return *m_layer; }
		dd::Job* Job() const { return m_job; }

		const UpdateBufferView& Data(const char* name = nullptr) const;

	private:
		friend struct System;

		dd::Job* m_job { nullptr };
		ddc::EntityLayer* m_layer { nullptr };
		float m_delta { 0 };
		dd::Array<UpdateBufferView, 8> m_views;

		const UpdateBufferView& CreateView(UpdateBuffer& buffer, size_t start, size_t count);
	};
}
