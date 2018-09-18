#pragma once

#include "ComponentBuffer.h"

namespace ddc
{
	struct DataRequest;
	struct World;

	struct UpdateData
	{
		UpdateData( ddc::World& world, dd::Span<Entity> entities, const dd::IArray<const DataRequest*>& requests, float delta_t );

		float Delta() const { return m_delta; }
		ddc::World& World() const { return m_world; }
		dd::Span<Entity> Entities() const { return m_entities; }
		size_t Size() const { return m_entities.Size(); }

		template <typename T>
		ddc::ReadView<T> Read() const
		{
			for( const ComponentBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == T::Type )
				{
					DD_ASSERT( buffer.Usage() == DataUsage::Read );
					return ddc::ReadView<T>( buffer );
				}
			}
			throw std::exception( "No read buffer found for component. Check your requests!" );
		}

		template <typename T>
		ddc::WriteView<T> Write() const
		{
			for( const ComponentBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == T::Type )
				{
					DD_ASSERT( buffer.Usage() == DataUsage::Write );
					return ddc::WriteView<T>( buffer );
				}
			}
			throw std::exception( "No write buffer found for component. Check your requests!" );
		}

		void Commit();

	private:

		float m_delta { 0 };
		ddc::World& m_world;
		dd::Span<Entity> m_entities;
		std::vector<ComponentBuffer> m_buffers;
	};
}