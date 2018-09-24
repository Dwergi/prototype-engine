//
// RenderData.cpp - Data used for renderer updates.
// Copyright (C) Sebastian Nordgren 
// August 26th 2018
//

#include "PrecompiledHeader.h"
#include "RenderData.h"

#include "DataRequest.h"
#include "World.h"
#include "ICamera.h"

namespace ddr
{
	RenderData::RenderData( ddc::World& world, const ddr::ICamera& camera, ddr::UniformStorage& uniforms,
		const std::vector<ddc::Entity>& entities, const dd::IArray<const ddc::DataRequest*>& requests  ) :
		m_camera( camera ),
		m_uniforms( uniforms ),
		m_entities( entities ),
		m_world( world )
	{
		m_buffers.reserve( requests.Size() );

		for( const ddc::DataRequest* req : requests )
		{
			byte* storage = req->Buffer();

			ddc::ComponentBuffer buffer( world, entities, *req );
			m_buffers.push_back( buffer );
		}

		DD_ASSERT( m_buffers.size() > 0, "No requirements registered." );
	}
}