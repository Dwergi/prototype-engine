#include "PrecompiledHeader.h"
#include "DataRequest.h"

#include "ComponentType.h"
#include "System.h"

namespace ddc
{
	DataRequest::DataRequest( const ComponentType& component, DataUsage usage, bool optional ) :
		m_component( &component ),
		m_usage( usage ),
		m_optional( optional )
	{
		m_buffer = new byte[ component.Size * MAX_ENTITIES ];
	}

	DataRequest::DataRequest( const DataRequest& other )
	{
		m_component = other.m_component;
		m_usage = other.m_usage;
		m_buffer = other.m_buffer;
		m_optional = other.m_optional;
	}
}