#include "PrecompiledHeader.h"
#include "DataRequirement.h"

#include "ComponentType.h"
#include "System.h"

namespace ddc
{
	DataRequirement::DataRequirement( const ComponentType& component, DataUsage usage ) :
		m_component( &component ),
		m_usage( usage )
	{
		m_buffer = new byte[ component.Size * MAX_ENTITIES ];
	}

	DataRequirement::DataRequirement( const DataRequirement& other )
	{
		m_component = other.m_component;
		m_usage = other.m_usage;
		m_buffer = other.m_buffer;
	}
}