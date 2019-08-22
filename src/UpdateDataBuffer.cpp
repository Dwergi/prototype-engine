#include "PCH.h"
#include "UpdateDataBuffer.h"

namespace ddc
{
	UpdateDataBuffer::UpdateDataBuffer(const char* name)
	{
		if (name != nullptr)
		{
			m_name = name;
		}
	}

	UpdateDataBuffer::UpdateDataBuffer(UpdateDataBuffer&& other) :
		m_name(other.m_name),
		m_entities(std::move(other.m_entities)),
		m_buffers(std::move(other.m_buffers)),
		m_requests(std::move(other.m_requests)),
		m_requiredComponents(std::move(other.m_requiredComponents)),
		m_tags(std::move(other.m_tags))
	{
	}

	void UpdateDataBuffer::RequestData(DataRequest* request)
	{
		if (CheckDuplicates(request->Component(), request->Usage(), request->Cardinality()))
		{
			m_requests.Add(request);
			m_buffers.Add(ComponentBuffer(request));

			if (!request->Optional())
			{
				m_requiredComponents.Add(request->Component().ComponentID());
			}
		}
		else
		{
			delete request;
		}
	}

	void UpdateDataBuffer::Fill(ddc::EntityLayer& layer)
	{
		m_entities.clear();
		layer.FindAllWith(m_requiredComponents, m_tags, m_entities);

		for (ComponentBuffer& buffer : m_buffers)
		{
			buffer.Fill(m_entities);
		}
	}

	bool UpdateDataBuffer::CheckDuplicates(const dd::TypeInfo& component, ddc::DataUsage usage, ddc::DataCardinality cardinality) const
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

	void UpdateDataBuffer::Commit()
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
}