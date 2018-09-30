//
// UpdateData.cpp
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#include "PCH.h"
#include "UpdateData.h"

namespace ddc
{
	DataBuffer::DataBuffer( ddc::World& world, const std::vector<Entity>& entities, const dd::IArray<const DataRequest*>& requests, const char* name ) :
		m_entities( entities )
	{
		m_buffers.reserve( MAX_BUFFERS );

		if( name != nullptr )
		{
			m_name = name;
		}

		for( const DataRequest* req : requests )
		{
			DD_ASSERT( req->Name() == m_name );

			ComponentBuffer component_buffer( world, entities, *req );
			m_buffers.push_back( component_buffer );
		}
	}

	DataBuffer::DataBuffer( const DataBuffer& other ) :
		m_name( other.m_name ),
		m_entities( other.m_entities ),
		m_buffers( other.m_buffers )
	{
	}

	UpdateData::UpdateData( ddc::World& world, float delta_t ) :
		m_world( world ),
		m_delta( delta_t )
	{
		m_dataBuffers.reserve( MAX_BUFFERS );
	}

	void UpdateData::AddData( const std::vector<Entity>& entities, const dd::IArray<const DataRequest*>& requests, const char* name )
	{
		DataBuffer buffer( m_world, entities, requests, name );
		m_dataBuffers.push_back( buffer );
	}

	const DataBuffer& UpdateData::Data( const char* name ) const
	{
		dd::String16 str;
		if( name != nullptr )
		{
			str = name;
		}

		for( const DataBuffer& data_buffer : m_dataBuffers )
		{
			if( data_buffer.Name() == str )
			{
				return data_buffer;
			}
		}
		throw std::exception( "No DataBuffer found for given name!" );
	}

	void UpdateData::Commit()
	{
		for( const DataBuffer& data_buffer : m_dataBuffers )
		{
			for( const ComponentBuffer& buffer : data_buffer.ComponentBuffers() )
			{
				if( buffer.Usage() != DataUsage::Write )
				{
					continue;
				}

				const byte* src = buffer.Data();
				const size_t cmp_size = buffer.Component().Size();

				for( Entity entity : data_buffer.Entities() )
				{
					void* dest = m_world.AccessComponent( entity, buffer.Component().ComponentID() );
					if( dest != nullptr )
					{
						memcpy( dest, src, cmp_size );
					}

					src += cmp_size;
				}
			}
		}
	}

	/*void CleverCopy( ComponentBuffer& buffer )
	{
		const byte* src = buffer.Data();
		size_t copy_size = 0;
		byte* dest_start = nullptr;

		const size_t cmp_size = buffer.Component().Size;

		for( Entity entity : m_entities )
		{
			void* dest = m_world.AccessComponent( entity, buffer.Component().ID );
			DD_ASSERT( buffer.Optional() || dest != nullptr );

			if( dest == nullptr )
			{
				if( copy_size > 0 )
				{
					memcpy( dest_start, src, copy_size );
					src += copy_size + cmp_size;

					dest_start = nullptr;
					copy_size = 0;
				}
				continue;
			}

			if( dest_start == nullptr )
			{
				dest_start = (byte*) dest;
			}
			else if( dest != (dest_start + copy_size) )
			{
				// discontinuity, commit the copy
				memcpy( dest_start, src, copy_size );

				src += copy_size;
				dest_start = (byte*) dest;
				copy_size = 0;
			}

			copy_size += cmp_size;
		}

		// final copy
		if( copy_size > 0 )
		{
			memcpy( dest_start, src, copy_size );
		}
	}*/
}