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

	DataRequest::~DataRequest()
	{
		byte* buffer = m_buffer.Release();
		free(buffer);
	}

	byte* DataRequest::GetBuffer(size_t count)
	{
		size_t required_size = m_component->Size() * count;
		if (m_buffer.Size() < required_size)
		{
			byte* old_buffer = m_buffer.Release();
			byte* new_buffer = (byte*) realloc(old_buffer, required_size);
			m_buffer.Set(new_buffer, (int) required_size);
		}

		return m_buffer.Access();
	}
}