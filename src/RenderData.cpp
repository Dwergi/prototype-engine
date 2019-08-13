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
	RenderData::RenderData(ddc::EntitySpace& space, const ddr::ICamera& camera, ddr::UniformStorage& uniforms,
		const std::vector<ddc::Entity>& entities, const dd::IArray<ddc::DataRequest*>& requests, float delta_t) :
		m_space(space),
		m_camera(camera),
		m_uniforms(uniforms),
		m_entities(entities),
		m_delta(delta_t)
	{
		m_buffers.reserve(requests.Size());

		for (ddc::DataRequest* req : requests)
		{
			byte* storage = req->GetBuffer(entities.size());

			ddc::ComponentBuffer buffer(space, entities, *req);
			m_buffers.push_back(buffer);
		}
	}

	RenderData::RenderData(RenderData&& other) :
		m_space(other.m_space),
		m_camera(other.m_camera),
		m_uniforms(other.m_uniforms),
		m_entities(std::move(other.m_entities)),
		m_buffers(std::move(other.m_buffers)),
		m_delta(other.m_delta)
	{

	}
}