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
		if (!IsEnabledForLayer(layer))
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

	bool System::CheckDuplicates(const dd::TypeInfo* component, ddc::DataUsage usage, ddc::DataCardinality cardinality, const char* name) const
	{
		for( const ddc::DataRequest* req : m_requests )
		{
			if( req->Component() == *component &&
				req->Usage() == usage && 
				req->Cardinality() == cardinality && 
				req->Name() == name )
			{
				DD_ASSERT( false, "Duplicate DataRequest found!" );
				return false;
			}
		}

		return true;
	}

	void System::RequireTag( ddc::Tag tag, const char* name )
	{
		ddc::UpdateDataBuffer& data_buffer = m_updateData.Create(name);
		data_buffer.RequireTag(tag);
	}
}