//
// DataRequirement.h - A wrapper around a data requirement for a system.
// Copyright (C) Sebastian Nordgren 
// July 3rd 2018
//

#pragma once

#include "Entity.h"


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
		const dd::TypeInfo& Component() const { DD_ASSERT( m_component != nullptr ); return *m_component; }

	protected:
		DataRequest( const dd::TypeInfo* component, DataUsage usage, DataCardinality cardinality, const char* name );

	private:
		const dd::TypeInfo* m_component { nullptr };
		DataUsage m_usage { DataUsage::Invalid };
		DataCardinality m_cardinality { DataCardinality::Invalid };

		dd::String16 m_name;
		byte* m_buffer { nullptr };
	};

	template <typename TComponent>
	struct ReadRequirement : DataRequest
	{
		ReadRequirement( const char* name ) :
			DataRequest( DD_TYPE( TComponent ), DataUsage::Read, DataCardinality::Required, name )
		{
		}
	};

	template <typename TComponent>
	struct WriteRequirement : DataRequest
	{
		WriteRequirement( const char* name ) :
			DataRequest( DD_TYPE( TComponent ), DataUsage::Write, DataCardinality::Required, name )
		{
		}
	};

	template <typename TComponent>
	struct ReadOptional : DataRequest
	{
		ReadOptional( const char* name ) :
			DataRequest( DD_TYPE( TComponent ), DataUsage::Read, DataCardinality::Optional, name )
		{
		}
	};

	template <typename TComponent>
	struct WriteOptional : DataRequest
	{
		WriteOptional( const char* name ) :
			DataRequest( DD_TYPE( TComponent ), DataUsage::Write, DataCardinality::Optional, name )
		{
		}
	};
}