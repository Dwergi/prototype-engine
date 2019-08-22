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

	ComponentBuffer::ComponentBuffer(ComponentBuffer&& other) :
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

		const size_t component_size = m_request->Component().Size();
		const dd::ComponentID component_id = m_request->Component().ComponentID();
		
		byte* dest = m_storage;

		std::memset(dest, 0, component_size * m_count);

		size_t copy_size = 0;
		byte* copy_start = nullptr;

		for (size_t i = 0; i < m_count; ++i)
		{
			const void* src = entities[i].GetComponent(component_id);
			DD_ASSERT(m_request->Optional() || src != nullptr);

			if (src != nullptr)
			{
				m_exists.push_back(true);

				if (copy_start == nullptr)
				{
					copy_start = (byte*) src;
				}

				copy_size += component_size;
			}
			else
			{
				m_exists.push_back(false);

				if (copy_start != nullptr && copy_size > 0)
				{
					std::memcpy(dest, copy_start, copy_size);

					dest += copy_size;
					copy_size = 0;
					copy_start = nullptr;
				}
			}
		}

		if (copy_start != nullptr && copy_size > 0)
		{
			std::memcpy(dest, copy_start, copy_size);
		}
	}

	void ComponentBuffer::Commit(const std::vector<ddc::Entity>& entities)
	{
		const size_t cmp_size = m_request->Component().Size();
		const dd::ComponentID cmp_id = m_request->Component().ComponentID();

		size_t copy_size = 0;
		byte* dest_start = nullptr;
		const byte* src = m_storage;

		for (Entity entity : entities)
		{
			void* dest = entity.AccessComponent(cmp_id);
			if (dest != nullptr)
			{
				if (dest_start == nullptr)
				{
					dest_start = (byte*) dest;
				}

				copy_size += cmp_size;
			}
			else if (dest_start != nullptr && copy_size > 0)
			{
				std::memcpy(dest_start, src, copy_size);

				src += copy_size;
				copy_size = 0;
				dest_start = nullptr;
			}
		}

		if (dest_start != nullptr && copy_size > 0)
		{
			std::memcpy(dest_start, src, copy_size);
		}
	}
}