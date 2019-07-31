//
// UpdateData.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "ComponentBuffer.h"

namespace ddc
{
	struct DataRequest;
	struct World;

	struct DataBuffer
	{
		DataBuffer( ddc::World& world, const std::vector<Entity>& entities, const dd::IArray<const DataRequest*>& requests, const char* name );
		DataBuffer( const DataBuffer& other );

		const std::vector<Entity>& Entities() const { return m_entities; }
		size_t Size() const { return m_entities.size(); }
		const dd::String& Name() const { return m_name; }

		const std::vector<ComponentBuffer>& ComponentBuffers() const { return m_buffers; }

		template <typename T>
		ddc::ReadView<T> Read() const
		{
			const dd::TypeInfo* type = DD_FIND_TYPE( T );

			for( const ComponentBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == *type &&
					buffer.Usage() == DataUsage::Read )
				{
					return ddc::ReadView<T>( buffer );
				}
			}
			throw std::exception( "No read buffer found for component. Check your requests!" );
		}

		template <typename T>
		ddc::WriteView<T> Write() const
		{
			const dd::TypeInfo* type = DD_FIND_TYPE( T );

			for( const ComponentBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == *type &&
					buffer.Usage() == DataUsage::Write )
				{
					return ddc::WriteView<T>( buffer );
				}
			}
			throw std::exception( "No write buffer found for component. Check your requests!" );
		}

	private:

		static const int MAX_BUFFERS = 16;

		dd::String16 m_name;
		std::vector<Entity> m_entities;
		std::vector<ComponentBuffer> m_buffers;
	};

	struct UpdateData
	{
		UpdateData( ddc::World& world, float delta_t );
		UpdateData( const UpdateData& other ) = delete;

		void AddData( const std::vector<Entity>& entities, const dd::IArray<const DataRequest*>& requests, const char* name );

		float Delta() const { return m_delta; }
		ddc::World& World() const { return m_world; }
		const DataBuffer& Data( const char* name = nullptr ) const;

		void Commit();

	private:

		static const int MAX_BUFFERS = 16;

		float m_delta { 0 };
		ddc::World& m_world;
		std::vector<DataBuffer> m_dataBuffers;
	};
}