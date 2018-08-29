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
	RenderData::RenderData( ddc::World& world, dd::Span<ddc::Entity> entities, const dd::IArray<const ddc::DataRequirement*>& requirements, ddr::ICamera& camera, ddr::UniformStorage& uniforms ) :
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

			ddc::ComponentDataBuffer data_buffer( entities, world, req->Component(), req->Usage(), storage );
			m_buffers.push_back( data_buffer );
		}
	}
}