//
// ComponentBuffer.cpp
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#include "PCH.h"
#include "ComponentBuffer.h"

namespace ddc
{
	ComponentBuffer::ComponentBuffer( const World& world, const std::vector<Entity>& entities, const DataRequest& req ) :
		m_request( req )
	{
		m_count = entities.size();
		m_storage = req.Buffer();

		if( req.Optional() )
		{
			m_exists.reserve( m_count );
		}

		size_t component_size = req.Component().Size();
		byte* dest = m_storage;

		for( size_t i = 0; i < m_count; ++i )
		{
			const void* src = world.GetComponent( entities[i], req.Component().ComponentID() );
			DD_ASSERT( req.Optional() || src != nullptr );

			if( src != nullptr )
			{
				memcpy( dest, src, component_size );

				if( req.Optional() )
				{
					m_exists.push_back( true );
				}
			}
			else
			{
				memset( dest, 0, component_size );

				if( req.Optional() )
				{
					m_exists.push_back( false );
				}
			}

			dest += component_size;
		}

		DD_ASSERT( m_exists.size() == 0 || req.Optional() );
	}
}