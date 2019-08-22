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
	UpdateDataBuffer& UpdateData::Create(const char* name)
	{
		dd::String16 str_name(name);

		for (UpdateDataBuffer& buffer : m_dataBuffers)
		{
			if (buffer.Name() == str_name)
			{
				return buffer;
			}
		}

		return m_dataBuffers.Add(UpdateDataBuffer(name));
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

	void UpdateData::Fill(ddc::EntityLayer& layer, float delta_t)
	{
		m_layer = &layer;
		m_delta = delta_t;

		for (UpdateDataBuffer& buffer : m_dataBuffers)
		{
			buffer.Fill(layer);
		}
	}

	void UpdateData::Commit()
	{
		for (UpdateDataBuffer& data_buffer : m_dataBuffers)
		{
			data_buffer.Commit();
		}
	}
}
