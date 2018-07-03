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

	struct ComponentType;
	struct System;

	struct DataRequirement
	{
		DataRequirement( const DataRequirement& other );

		virtual ~DataRequirement()
		{
			delete[] m_buffer;
		}

		DataUsage Usage() const { return m_usage; }
		const ComponentType& Component() const { return *m_component; }
		byte* GetBuffer() const { return m_buffer; }

	protected:
		DataRequirement( const ComponentType& component, DataUsage usage, System& system );

	private:
		const ComponentType* m_component { 0 };
		DataUsage m_usage { DataUsage::Invalid };
		byte* m_buffer { nullptr };
	};

	template <typename TComponent>
	struct ReadRequirement : DataRequirement
	{
		ReadRequirement( System& system ) :
			DataRequirement( TComponent::Type, DataUsage::Read, system )
		{
		}
	};

	template <typename TComponent>
	struct WriteRequirement : DataRequirement
	{
		WriteRequirement( System& system ) :
			DataRequirement( TComponent::Type, DataUsage::Write, system )
		{
		}
	};
}