#pragma once

#include "ComponentDataBuffer.h"

namespace ddc
{
	struct DataRequirement;
	struct EntityLayer;

	struct UpdateData
	{
		UpdateData( EntityLayer& layer, dd::Span<Entity> entities, const dd::IArray<const DataRequirement*>& requirements );

		template <typename T>
		ReadBuffer<T> GetRead() const
		{
			for( const ComponentDataBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == T::Type )
				{
					DD_ASSERT( buffer.Usage() == DataUsage::Read );

					return ReadBuffer<T>( buffer );
				}
			}

			throw std::exception( "No read buffer found for component. Check your requirements!" );
		}

		template <typename T>
		WriteBuffer<T> GetWrite() const
		{
			for( const ComponentDataBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == T::Type )
				{
					DD_ASSERT( buffer.Usage() == DataUsage::Write );

					return WriteBuffer<T>( buffer );
				}
			}

			throw std::exception( "No write buffer found for component. Check your requirements!" );
		}

		template <typename T>
		ReadWriteBuffer<T> GetReadWrite() const
		{
			for( const ComponentDataBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == T::Type )
				{
					DD_ASSERT( buffer.Usage() == DataUsage::ReadWrite );

					return ReadWriteBuffer<T>( buffer );
				}
			}

			throw std::exception( "No read-write buffer found for component. Check your requirements!" );
		}

		void Commit();

	private:

		size_t m_entityCount { 0 };

		EntityLayer& m_layer;
		dd::Span<Entity> m_entities;
		std::vector<ComponentDataBuffer> m_buffers;
	};
}