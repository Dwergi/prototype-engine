#include "PCH.h"
#include "DataRequest.h"

namespace ddc
{
	DataRequest::DataRequest( const dd::TypeInfo* component, DataUsage usage, DataCardinality cardinality, const char* name ) :
		m_component( component ),
		m_usage( usage ),
		m_cardinality( cardinality )
	{
		if( name != nullptr )
		{
			m_name = name;
		}

		m_buffer = new byte[ component->Size() * MAX_ENTITIES ];

		DD_ASSERT( m_component != nullptr );
	}

	DataRequest::DataRequest( const DataRequest& other )
	{
		m_component = other.m_component;
		m_usage = other.m_usage;
		m_buffer = other.m_buffer;
		m_cardinality = other.m_cardinality;
		m_name = other.m_name;

		DD_ASSERT( m_component != nullptr );
	}
}