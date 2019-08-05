#include "PCH.h"
#include "UpdateDataBuffer.h"

namespace ddc
{
	UpdateDataBuffer::UpdateDataBuffer(ddc::EntitySpace& space, const std::vector<Entity>& entities, const dd::IArray<const DataRequest*>& requests, const char* name) :
		m_entities(entities)
	{
		m_buffers.reserve(MAX_BUFFERS);

		if (name != nullptr)
		{
			m_name = name;
		}

		for (const DataRequest* req : requests)
		{
			DD_ASSERT(req->Name() == m_name);

			ComponentBuffer component_buffer(space, entities, *req);
			m_buffers.push_back(component_buffer);
		}
	}

	UpdateDataBuffer::UpdateDataBuffer(const UpdateDataBuffer& other) :
		m_name(other.m_name),
		m_entities(other.m_entities),
		m_buffers(other.m_buffers)
	{
	}
}