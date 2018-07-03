#include "PrecompiledHeader.h"
#include "UpdateData.h"

namespace ddc
{
	UpdateData::UpdateData( EntityLayer& layer, dd::Span<Entity> entities, const dd::IArray<const DataRequirement*>& requirements ) :
		m_layer( layer ),
		m_entities( entities )
	{
		m_buffers.reserve( requirements.Size() );

		size_t entity_offset = entities.Offset();

		for( const DataRequirement* req : requirements )
		{
			byte* storage = req->GetBuffer() + (entity_offset * req->Component().Size);

			ComponentDataBuffer data_buffer( entities, layer, req->Component(), req->Usage(), storage );
			m_buffers.push_back( data_buffer );
		}
	}

	void UpdateData::Commit()
	{
		for( ComponentDataBuffer& buffer : m_buffers )
		{
			buffer.Commit( m_entities, m_layer );
		}
	}
}