//
// System.cpp
// Copyright (C) Sebastian Nordgren 
// September 24th 2018
//

#include "PCH.h"
#include "System.h"

namespace ddc
{
	System::System(const char* name) :
		m_name(name)
	{
	}

	void System::EnableForLayer(const ddc::EntityLayer& layer)
	{
		if (m_layers.empty() || !IsEnabledForLayer(layer))
		{
			m_layers.push_back(&layer);
		}
	}

	bool System::IsEnabledForLayer(const ddc::EntityLayer& layer) const
	{
		if (!m_enabled)
		{
			return false;
		}

		if (m_layers.empty())
		{
			return true;
		}

		auto it = std::find(m_layers.begin(), m_layers.end(), &layer);
		return it != m_layers.end();
	}

	void System::RequireTag(ddc::Tag tag, const char* name)
	{
		ddc::UpdateBuffer& update_buffer = CreateBuffer(name);
		update_buffer.RequireTag(tag);
	}

	UpdateBuffer& System::CreateBuffer(const char* name)
	{
		dd::String16 str_name(name);

		for (UpdateBuffer& buffer : m_updateBuffers)
		{
			if (buffer.Name() == str_name)
			{
				return buffer;
			}
		}

		return m_updateBuffers.Add(UpdateBuffer(name));
	}

	ddc::UpdateData System::CreateUpdateData(ddc::EntityLayer& layer, float delta_t) const
	{
		ddc::UpdateData data(layer, delta_t);

		for (UpdateBuffer& buffer : m_updateBuffers)
		{
			buffer.Fill(layer);

			DD_TODO("This isn't segmented for multithreaded updates...");

			data.CreateView(buffer, 0, buffer.Size());
		}

		return std::move(data);
	}

	void System::CommitChanges(ddc::UpdateData& data)
	{
		for (UpdateBuffer& buffer : m_updateBuffers)
		{
			buffer.Commit();
		}

		data.CommitChanges();
	}
}