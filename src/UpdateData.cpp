//
// UpdateData.cpp
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#include "PCH.h"
#include "UpdateData.h"

#include "Input.h"

namespace ddc
{
	UpdateData::UpdateData(ddc::EntitySpace& space, float delta_t) :
		m_entitySpace(space),
		m_delta(delta_t)
	{
	}

	void UpdateData::AddData(const std::vector<Entity>& entities, const dd::IArray<DataRequest*>& requests, const char* name)
	{
		UpdateDataBuffer buffer(m_entitySpace, entities, requests, name);
		m_dataBuffers.push_back(buffer);
	}

	const UpdateDataBuffer& UpdateData::Data(const char* name) const
	{
		dd::String16 str;
		if (name != nullptr)
		{
			str = name;
		}

		for (const UpdateDataBuffer& data_buffer : m_dataBuffers)
		{
			if (data_buffer.Name() == str)
			{
				return data_buffer;
			}
		}
		throw std::exception("No UpdateDataBuffer found for given name!");
	}

	void UpdateData::Commit()
	{
		for (const UpdateDataBuffer& data_buffer : m_dataBuffers)
		{
			for (const ComponentBuffer& buffer : data_buffer.ComponentBuffers())
			{
				if (buffer.Usage() != DataUsage::Write)
				{
					continue;
				}

				const byte* src = buffer.Data();
				const size_t cmp_size = buffer.Component().Size();

				for (Entity entity : data_buffer.Entities())
				{
					void* dest = m_entitySpace.AccessComponent(entity, buffer.Component().ComponentID());
					if (dest != nullptr)
					{
						memcpy(dest, src, cmp_size);
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