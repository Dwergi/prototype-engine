#include "PCH.h"
#include "UpdateDataBuffer.h"

namespace ddc
{
	UpdateDataBuffer::UpdateDataBuffer(const char* name)
	{
		m_buffers.reserve(MAX_BUFFERS);

		if (name != nullptr)
		{
			m_name = name;
		}
	}

	UpdateDataBuffer::UpdateDataBuffer(UpdateDataBuffer&& other) :
		m_name(other.m_name),
		m_entities(std::move(other.m_entities)),
		m_buffers(std::move(other.m_buffers)),
		m_tags(std::move(other.m_tags))
	{
	}

	void UpdateDataBuffer::RequireTag(ddc::Tag tag)
	{
		m_tags.set((int) tag);
	}

	void UpdateDataBuffer::AddRequest(const DataRequest& request)
	{
		CheckDuplicates(type, DataUsage::Write, DataCardinality::Optional, name);
		m_requests.Add(new WriteOptional<T>(name));

		dd::String16 name_str(name);
		if (!m_requestNames.Contains(name_str))
		{
			m_requestNames.Add(name_str);
		}
		m_requests.Add(request);
	}

	void UpdateDataBuffer::Fill(ddc::EntityLayer& layer)
	{
		for (DataRequest* req : m_requests)
		{
			DD_ASSERT(req->Name() == m_name);

			ComponentBuffer component_buffer(layer, m_entities, *req);
			m_buffers.push_back(component_buffer);
		}
	}

	bool UpdateDataBuffer::CheckDuplicates(const dd::TypeInfo* component, ddc::DataUsage usage, ddc::DataCardinality cardinality) const
	{
		for (const DataRequest* req : m_requests)
		{
			if (req->Component() == *component &&
				req->Usage() == usage &&
				req->Cardinality() == cardinality)
			{
				DD_ASSERT(false, "Duplicate DataRequest found!");
				return false;
			}
		}

		return true;
	}
}