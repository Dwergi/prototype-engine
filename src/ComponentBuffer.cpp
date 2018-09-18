#include "PrecompiledHeader.h"
#include "ComponentBuffer.h"

namespace ddc
{
	ComponentBuffer::ComponentBuffer( const World& world, dd::Span<Entity> entities, const DataRequest& req ) :
		m_request( req )
	{
		m_count = entities.Size();
		m_storage = req.Buffer() + entities.Offset();

		if( req.Optional() )
		{
			m_exists.reserve( m_count );
		}

		byte* dest = m_storage;

		for( size_t i = 0; i < m_count; ++i )
		{
			const void* src = world.GetComponent( entities[i], req.Component().ID );
			DD_ASSERT( req.Optional() || src != nullptr );

			if( src != nullptr )
			{
				memcpy( dest, src, req.Component().Size );

				if( req.Optional() )
				{
					m_exists.push_back( true );
				}
			}
			else
			{
				memset( dest, 0, req.Component().Size );

				if( req.Optional() )
				{
					m_exists.push_back( false );
				}
			}

			dest += req.Component().Size;
		}

		DD_ASSERT( m_exists.size() == 0 || req.Optional() );
	}
}