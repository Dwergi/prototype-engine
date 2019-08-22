#include "PCH.h"
#include "UpdateDataBuffer.h"

namespace ddc
{
	UpdateDataBuffer::UpdateDataBuffer(ddc::EntityLayer& layer, std::vector<Entity>&& entities, const dd::IArray<DataRequest*>& requests, const char* name) :
		m_entities(std::move(entities))
	{
		m_buffers.reserve(MAX_BUFFERS);

		if (name != nullptr)
		{
			m_name = name;
		}

		for (DataRequest* req : requests)
		{
			DD_ASSERT(req->Name() == m_name);

			ComponentBuffer component_buffer(layer, m_entities, *req);
			m_buffers.push_back(component_buffer);
		}
	}

	UpdateDataBuffer::UpdateDataBuffer(UpdateDataBuffer&& other) :
		m_name(other.m_name),
		m_entities(std::move(other.m_entities)),
		m_buffers(std::move(other.m_buffers))
	{
	}
}