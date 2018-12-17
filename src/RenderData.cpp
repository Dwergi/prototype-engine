//
// RenderData.cpp - Data used for renderer updates.
// Copyright (C) Sebastian Nordgren 
// August 26th 2018
//

#include "PCH.h"
#include "RenderData.h"

#include "DataRequest.h"
#include "World.h"
#include "ICamera.h"

namespace ddr
{
	RenderData::RenderData( const ddc::World& world, const ddr::ICamera& camera, 
		ddr::UniformStorage& uniforms, ddr::CommandBuffer& commands,
		const std::vector<ddc::Entity>& entities, const dd::IArray<const ddc::DataRequest*>& requests  ) :
		m_camera( camera ),
		m_uniforms( uniforms ),
		m_entities( entities ),
		m_world( world ),
		m_commands( commands )
	{
		m_buffers.reserve( requests.Size() );

		for( const ddc::DataRequest* req : requests )
		{
			byte* storage = req->Buffer();

			ddc::ComponentBuffer buffer( world, entities, *req );
			m_buffers.push_back( buffer );
		}
	}
}