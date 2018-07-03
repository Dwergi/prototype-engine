#include "PrecompiledHeader.h"
#include "ComponentDataBuffer.h"

namespace ddc
{
	ComponentDataBuffer::ComponentDataBuffer( dd::Span<Entity> entities, EntityLayer& layer, const ComponentType& component, DataUsage usage, byte* storage ) :
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
			const void* src = layer.GetComponent( entity, m_component.ID );
			memcpy( dest, src, m_component.Size );

			dest += m_component.Size;
		}
	}

	void ComponentDataBuffer::Commit( dd::Span<Entity> entities, EntityLayer& layer )
	{
		if( m_usage != DataUsage::Write )
		{
			return;
		}

		const byte* src = m_storage;

		for( Entity entity : entities )
		{
			void* dest = layer.AccessComponent( entity, m_component.ID );
			memcpy( dest, src, m_component.Size );

			src += m_component.Size;
		}
	}
}