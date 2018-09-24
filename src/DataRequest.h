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

	enum class DataCardinality
	{
		Invalid,
		Required,
		Optional
	};

	struct System;

	struct DataRequest
	{
		DataRequest( const DataRequest& other );

		virtual ~DataRequest() { delete[] m_buffer; }

		byte* Buffer() const { return m_buffer; }
		const dd::String& Name() const { return m_name; }
		DataCardinality Cardinality() const { return m_cardinality; }
		bool Optional() const { return m_cardinality == DataCardinality::Optional; }
		DataUsage Usage() const { return m_usage; }
		const ComponentType& Component() const { return *m_component; }

	protected:
		DataRequest( const ComponentType& component, DataUsage usage, DataCardinality cardinality, const char* name );

	private:
		const ComponentType* m_component { nullptr };
		DataUsage m_usage { DataUsage::Invalid };
		DataCardinality m_cardinality { DataCardinality::Invalid };

		dd::String16 m_name;
		byte* m_buffer { nullptr };
	};

	template <typename TComponent>
	struct ReadRequirement : DataRequest
	{
		ReadRequirement( const char* name ) :
			DataRequest( TComponent::Type, DataUsage::Read, DataCardinality::Required, name )
		{
		}
	};

	template <typename TComponent>
	struct WriteRequirement : DataRequest
	{
		WriteRequirement( const char* name ) :
			DataRequest( TComponent::Type, DataUsage::Write, DataCardinality::Required, name )
		{
		}
	};

	template <typename TComponent>
	struct ReadOptional : DataRequest
	{
		ReadOptional( const char* name ) :
			DataRequest( TComponent::Type, DataUsage::Read, DataCardinality::Optional, name )
		{
		}
	};

	template <typename TComponent>
	struct WriteOptional : DataRequest
	{
		WriteOptional( const char* name ) :
			DataRequest( TComponent::Type, DataUsage::Write, DataCardinality::Optional, name )
		{
		}
	};
}