//
// ComponentBuffer.cpp
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#include "PCH.h"
#include "ComponentBuffer.h"

namespace ddc
{
	ComponentBuffer::ComponentBuffer()
	{
	}

	ComponentBuffer::ComponentBuffer(DataRequest* request) :
		m_request(request)
	{
		DD_ASSERT(m_request != nullptr);
	}

	ComponentBuffer::ComponentBuffer(ComponentBuffer&& other) noexcept :
		m_request(other.m_request)
	{
		DD_ASSERT(m_request != nullptr);
	}

	void ComponentBuffer::Fill(const std::vector<Entity>& entities)
	{
		m_count = entities.size();
		m_storage = m_request->AccessBuffer(entities.size());
		
		m_exists.clear();
		m_exists.reserve(m_count);

		if (m_count == 0)
		{
			return;
		}

		const size_t cmp_size = m_request->Component()->Size();
		const dd::ComponentID cmp_id = m_request->Component()->ComponentID();
		
		byte* dest = m_storage;

		std::memset(dest, 0, cmp_size * m_count);

		for (size_t i = 0; i < m_count; ++i)
		{
			const void* src = entities[i].GetComponent(cmp_id);
			DD_ASSERT(m_request->Optional() || src != nullptr);

			if (src != nullptr)
			{
				m_exists.push_back(true);

				std::memcpy(dest, src, cmp_size);
			}
			else
			{
				m_exists.push_back(false);
			}

			dest += cmp_size;
		}
	}

	void ComponentBuffer::Commit(const std::vector<ddc::Entity>& entities)
	{
		const size_t cmp_size = m_request->Component()->Size();
		const dd::ComponentID cmp_id = m_request->Component()->ComponentID();

		const byte* src = m_storage;

		for (Entity entity : entities)
		{
			void* dest = entity.AccessComponent(cmp_id);
			if (dest != nullptr)
			{
				std::memcpy(dest, src, cmp_size);
			}

			src += cmp_size;
		}
	}
}