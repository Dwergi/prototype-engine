//
// RenderData.cpp - Data used for renderer updates.
// Copyright (C) Sebastian Nordgren 
// August 26th 2018
//

#include "PCH.h"
#include "RenderData.h"

namespace ddr
{
	RenderData::RenderData() :
		ddc::UpdateBuffer(nullptr)
	{
	}

	RenderData::RenderData(RenderData&& other) :
		ddc::UpdateBuffer(std::move(other))
	{
	}

	void RenderData::Fill(ddc::EntityLayer& layer, const ddr::ICamera& camera, ddr::UniformStorage& uniforms, float delta_t)
	{
		m_layer = &layer;
		m_camera = &camera;
		m_uniforms = &uniforms;
		m_delta = delta_t;

		ddc::UpdateBuffer::Fill(layer);
	}
}