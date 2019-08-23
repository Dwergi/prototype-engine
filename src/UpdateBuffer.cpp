#include "PCH.h"
#include "UpdateBuffer.h"

namespace ddc
{
	UpdateBuffer::UpdateBuffer(const char* name)
	{
		if (name != nullptr)
		{
			m_name = name;
		}
	}

	UpdateBuffer::UpdateBuffer(UpdateBuffer&& other) :
		m_name(other.m_name),
		m_entities(std::move(other.m_entities)),
		m_buffers(std::move(other.m_buffers)),
		m_requests(std::move(other.m_requests)),
		m_requiredComponents(std::move(other.m_requiredComponents)),
		m_tags(std::move(other.m_tags))
	{
	}

	void UpdateBuffer::RequestData(DataRequest* request)
	{
		if (CheckDuplicates(request->Component(), request->Usage(), request->Cardinality()))
		{
			m_requests.Add(request);
			m_buffers.Add(ComponentBuffer(request));

			if (!request->Optional())
			{
				m_requiredComponents.Add(request->Component()->ComponentID());
			}
		}
		else
		{
			delete request;
		}
	}

	bool UpdateBuffer::CheckDuplicates(const dd::TypeInfo* component, ddc::DataUsage usage, ddc::DataCardinality cardinality) const
	{
		for (const DataRequest* req : m_requests)
		{
			if (req->Component() == component)
			{
				DD_ASSERT(false, "Duplicate DataRequest found!");
				return false;
			}
		}

		return true;
	}

	void UpdateBuffer::Fill(ddc::EntityLayer& layer)
	{
		m_entities.clear();
		layer.FindAllWith(m_requiredComponents, m_tags, m_entities);

		for (ComponentBuffer& buffer : m_buffers)
		{
			buffer.Fill(m_entities);
		}
	}

	void UpdateBuffer::Commit()
	{
		for (ComponentBuffer& buffer : m_buffers)
		{
			if (buffer.Usage() != DataUsage::Write)
			{
				continue;
			}

			buffer.Commit(m_entities);
		}
	}

	UpdateBufferView::UpdateBufferView(UpdateBuffer& buffer, size_t start, size_t count) :
		m_buffer(&buffer),
		m_start(start),
		m_count(count)
	{
		DD_ASSERT(m_start + m_count <= m_buffer->Size());

		m_entities = dd::Span(m_buffer->Entities(), m_count, m_start);
	}

	UpdateBufferView::UpdateBufferView(const UpdateBufferView& other) :
		m_buffer(other.m_buffer),
		m_start(other.m_start),
		m_count(other.m_count),
		m_entities(other.m_entities)
	{
	}
}