#include "PrecompiledHeader.h"
#include "ComponentBuffer.h"

namespace ddc
{
	ComponentBuffer::ComponentBuffer( dd::Span<Entity> entities, const World& world, const ComponentType& component, DataUsage usage, byte* storage ) :
		m_component( component ),
		m_usage( usage ),
		m_storage( storage )
	{
		DD_ASSERT( storage != nullptr );

		m_count = entities.Size();

		size_t buffer_size = entities.Size() * m_component.Size;
		byte* dest = m_storage;

		for( size_t i = 0; i < entities.Size(); ++i )
		{
			const void* src = world.GetComponent( entities[i], m_component.ID );
			DD_ASSERT( src != nullptr );

			memcpy( dest, src, m_component.Size );

			dest += m_component.Size;
		}
	}
}