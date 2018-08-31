//
// RenderData.cpp - Data used for renderer updates.
// Copyright (C) Sebastian Nordgren 
// August 26th 2018
//

#include "PrecompiledHeader.h"
#include "RenderData.h"

#include "DataRequirement.h"
#include "World.h"
#include "ICamera.h"

namespace ddr
{
	RenderData::RenderData( const ddc::World& world, const ddr::ICamera& camera, ddr::UniformStorage& uniforms,
		dd::Span<ddc::Entity> entities, const dd::IArray<const ddc::DataRequirement*>& requirements  ) :
		m_camera( camera ),
		m_uniforms( uniforms ),
		m_entities( entities ),
		m_world( world )
	{
		m_buffers.reserve( requirements.Size() );

		size_t entity_offset = entities.Offset();

		for( const ddc::DataRequirement* req : requirements )
		{
			byte* storage = req->GetBuffer() + (entity_offset * req->Component().Size);

			ddc::ComponentBuffer buffer( entities, world, req->Component(), ddc::DataUsage::Read, storage );
			m_buffers.push_back( buffer );
		}

		DD_ASSERT( m_buffers.size() > 0, "No requirements registered." );
	}
}