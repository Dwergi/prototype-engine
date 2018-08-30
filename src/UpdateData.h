#pragma once

#include "ComponentDataBuffer.h"

namespace ddc
{
	struct DataRequirement;
	struct World;

	struct UpdateData
	{
		UpdateData( ddc::World& world, dd::Span<Entity> entities, const dd::IArray<const DataRequirement*>& requirements );

		ddc::World& World() const { return m_world; }

		dd::Span<Entity> Entities() const { return m_entities; }

		size_t Size() const { return m_entities.Size(); }

		template <typename T>
		ReadBuffer<T> Read() const
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
		WriteBuffer<T> Write() const
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

		void Commit();

	private:

		ddc::World& m_world;
		dd::Span<Entity> m_entities;
		std::vector<ComponentDataBuffer> m_buffers;
	};
}