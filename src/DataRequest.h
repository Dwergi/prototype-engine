//
// DataRequirement.h - A wrapper around a data requirement for a system.
// Copyright (C) Sebastian Nordgren 
// July 3rd 2018
//

#pragma once

#include "ComponentType.h"
#include "Entity.h"

#include <bitset>

namespace ddc
{
	enum class DataUsage
	{
		Invalid,
		Read,
		Write
	};

	struct System;

	struct DataRequest
	{
		DataRequest( const DataRequest& other );

		virtual ~DataRequest() { delete[] m_buffer; }

		byte* Buffer() const { return m_buffer; }
		bool Optional() const { return m_optional; }
		DataUsage Usage() const { return m_usage; }
		const ComponentType& Component() const { return *m_component; }

	protected:
		DataRequest( const ComponentType& component, DataUsage usage, bool optional );

	private:
		const ComponentType* m_component { nullptr };
		DataUsage m_usage { DataUsage::Invalid };
		bool m_optional { false };
		byte* m_buffer { nullptr };
	};

	template <typename TComponent>
	struct ReadRequirement : DataRequest
	{
		ReadRequirement() :
			DataRequest( TComponent::Type, DataUsage::Read, false )
		{
		}
	};

	template <typename TComponent>
	struct WriteRequirement : DataRequest
	{
		WriteRequirement() :
			DataRequest( TComponent::Type, DataUsage::Write, false )
		{
		}
	};

	template <typename TComponent>
	struct ReadOptional : DataRequest
	{
		ReadOptional() :
			DataRequest( TComponent::Type, DataUsage::Read, true )
		{
		}
	};

	template <typename TComponent>
	struct WriteOptional : DataRequest
	{
		WriteOptional() :
			DataRequest( TComponent::Type, DataUsage::Write, true )
		{
		}
	};
}