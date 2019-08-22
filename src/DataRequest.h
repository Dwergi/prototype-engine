//
// DataRequirement.h - A wrapper around a data requirement for a system.
// Copyright (C) Sebastian Nordgren 
// July 3rd 2018
//

#pragma once

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

		virtual ~DataRequest();

		byte* GetBuffer(size_t count);
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
		dd::Buffer<byte> m_buffer;
	};

	template <typename TComponent>
	struct ReadRequired : DataRequest
	{
		ReadRequired() :
			DataRequest( DD_FIND_TYPE( TComponent ), DataUsage::Read, DataCardinality::Required )
		{
		}
	};

	template <typename TComponent>
	struct WriteRequired : DataRequest
	{
		WriteRequired() :
			DataRequest( DD_FIND_TYPE( TComponent ), DataUsage::Write, DataCardinality::Required )
		{
		}
	};

	template <typename TComponent>
	struct ReadOptional : DataRequest
	{
		ReadOptional() :
			DataRequest( DD_FIND_TYPE( TComponent ), DataUsage::Read, DataCardinality::Optional )
		{
		}
	};

	template <typename TComponent>
	struct WriteOptional : DataRequest
	{
		WriteOptional() :
			DataRequest( DD_FIND_TYPE( TComponent ), DataUsage::Write, DataCardinality::Optional )
		{
		}
	};
}