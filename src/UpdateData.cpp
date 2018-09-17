#include "PrecompiledHeader.h"
#include "UpdateData.h"

namespace ddc
{
	UpdateData::UpdateData( ddc::World& world, float delta_t, dd::Span<Entity> entities, const dd::IArray<const DataRequirement*>& requirements ) :
		m_world( world ),
		m_delta( delta_t ),
		m_entities( entities )
	{
		m_buffers.reserve( requirements.Size() );

		size_t entity_offset = entities.Offset();

		for( const DataRequirement* req : requirements )
		{
			byte* storage = req->GetBuffer() + (entity_offset * req->Component().Size);

			ComponentBuffer data_buffer( entities, world, req->Component(), req->Usage(), storage );
			m_buffers.push_back( data_buffer );
		}
	}

	void UpdateData::Commit()
	{
		for( ComponentBuffer& buffer : m_buffers )
		{
			if( buffer.Usage() != DataUsage::Write )
			{
				continue;
			}

			const byte* src = buffer.Data();
			size_t copy_size = 0;
			byte* dest_start = nullptr;

			for( Entity entity : m_entities )
			{
				void* dest = m_world.AccessComponent( entity, buffer.Component().ID );

				if( dest_start == nullptr )
				{
					dest_start = (byte*) dest;
				}
				else if( dest != (dest_start + copy_size) )
				{
					// discontinuity, commit the copy
					memcpy( dest_start, src, copy_size );

					src += copy_size;
					dest_start = (byte*) dest;
					copy_size = 0;
				}

				copy_size += buffer.Component().Size;
			}

			// final copy
			if( copy_size > 0 )
			{
				memcpy( dest_start, src, copy_size );
			}
		}
	}
}