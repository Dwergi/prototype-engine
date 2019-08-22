//
// RenderData.cpp - Data used for renderer updates.
// Copyright (C) Sebastian Nordgren 
// August 26th 2018
//

#include "PCH.h"
#include "RenderData.h"

#include "DataRequest.h"

#include "ICamera.h"

namespace ddr
{
	RenderData::RenderData()
	{
	}

	RenderData::RenderData(RenderData&& other) :
		m_layer(other.m_layer),
		m_camera(other.m_camera),
		m_uniforms(other.m_uniforms),
		m_entities(std::move(other.m_entities)),
		m_buffers(std::move(other.m_buffers)),
		m_delta(other.m_delta)
	{

	}

	void RenderData::Fill(ddc::EntityLayer& layer, const ddr::ICamera& camera, ddr::UniformStorage& uniforms,
		std::vector<ddc::Entity>&& entities, const dd::IArray<ddc::DataRequest*>& requests, float delta_t)
	{
		m_entities = std::move(entities);
		m_layer = &layer;
		m_camera = &camera;
		m_uniforms = &uniforms;
		m_delta = delta_t;

		m_buffers.reserve(requests.Size());

		for (ddc::DataRequest* req : requests)
		{
			m_buffers.emplace_back(m_layer, entities, *req);
		}
	}
}