//
// ComponentBuffer.cpp
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#include "PCH.h"
#include "ComponentBuffer.h"

namespace ddc
{
	ComponentBuffer::ComponentBuffer( const EntitySpace& space, const std::vector<Entity>& entities, DataRequest& req ) :
		m_request( req )
	{
		m_count = entities.size();
		m_storage = req.GetBuffer(entities.size());

		if( req.Optional() )
		{
			m_exists.resize( m_count );
		}

		size_t component_size = req.Component().Size();
		byte* dest = m_storage;

		memset(dest, 0, component_size * m_count);

		for( size_t i = 0; i < m_count; ++i )
		{
			const void* src = space.GetComponent( entities[i], req.Component().ComponentID() );
			DD_ASSERT( req.Optional() || src != nullptr );

			if( src != nullptr )
			{
				memcpy( dest, src, component_size );

				if( req.Optional() )
				{
					m_exists[i] = true;
				}
			}
			else
			{
				if( req.Optional() )
				{
					m_exists[i] = false;
				}
			}

			dest += component_size;
		}

		DD_ASSERT( m_exists.size() == 0 || req.Optional() );
	}
}