#include "PrecompiledHeader.h"
#include "ComponentDataBuffer.h"

namespace ddc
{
	ComponentDataBuffer::ComponentDataBuffer( dd::Span<Entity> entities, World& world, const ComponentType& component, DataUsage usage, byte* storage ) :
		m_component( component ),
		m_usage( usage ),
		m_storage( storage )
	{
		DD_ASSERT( storage != nullptr );

		m_count = entities.Size();

		size_t buffer_size = entities.Size() * m_component.Size;
		byte* dest = m_storage;

		for( Entity entity : entities )
		{
			const void* src = world.GetComponent( entity, m_component.ID );
			memcpy( dest, src, m_component.Size );

			dest += m_component.Size;
		}
	}

	void ComponentDataBuffer::Commit( dd::Span<Entity> entities, World& world )
	{
		if( m_usage != DataUsage::Write )
		{
			return;
		}

		const byte* src = m_storage;
		size_t copy_size = 0;
		byte* dest_start = nullptr;

		for( Entity entity : entities )
		{
			void* dest = world.AccessComponent( entity, m_component.ID );

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
			
			copy_size += m_component.Size;
		}

		// final copy
		if( copy_size > 0 )
		{
			memcpy( dest_start, src, copy_size );
		}
	}
}