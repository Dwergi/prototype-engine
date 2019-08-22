//
// UpdateData.cpp
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#include "PCH.h"
#include "UpdateData.h"

#include "Input.h"

namespace ddc
{
	UpdateData::UpdateData(ddc::EntityLayer& layer, float delta_t) :
		m_layer(layer),
		m_delta(delta_t)
	{
	}

	void UpdateData::AddData(std::vector<Entity>&& entities, const dd::IArray<DataRequest*>& requests, const char* name)
	{
		UpdateDataBuffer buffer(m_layer, std::move(entities), requests, name);
		m_dataBuffers.push_back(buffer);
	}

	const UpdateDataBuffer& UpdateData::Data(const char* name) const
	{
		dd::String16 str;
		if (name != nullptr)
		{
			str = name;
		}

		for (const UpdateDataBuffer& data_buffer : m_dataBuffers)
		{
			if (data_buffer.Name() == str)
			{
				return data_buffer;
			}
		}
		throw std::exception("No UpdateDataBuffer found for given name!");
	}

	void UpdateData::Commit()
	{
		for (const UpdateDataBuffer& data_buffer : m_dataBuffers)
		{
			for (const ComponentBuffer& buffer : data_buffer.ComponentBuffers())
			{
				if (buffer.Usage() != DataUsage::Write)
				{
					continue;
				}

				const size_t cmp_size = buffer.Component().Size();

				size_t copy_size = 0;
				byte* dest_start = nullptr;
				const byte* src = buffer.Data();

				for (Entity entity : data_buffer.Entities())
				{
					void* dest = m_layer.AccessComponent(entity, buffer.Component().ComponentID());
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
	}
}
