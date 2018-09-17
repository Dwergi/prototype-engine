#pragma once

#include "ComponentBuffer.h"

namespace ddc
{
	struct DataRequirement;
	struct World;

	struct UpdateData
	{
		UpdateData( ddc::World& world, float delta_t, dd::Span<Entity> entities, const dd::IArray<const DataRequirement*>& requirements );

		float Delta() const { return m_delta; }

		ddc::World& World() const { return m_world; }

		dd::Span<Entity> Entities() const { return m_entities; }

		size_t Size() const { return m_entities.Size(); }

		template <typename T>
		dd::ConstBuffer<T> Read() const
		{
			for( const ComponentBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == T::Type )
				{
					DD_ASSERT( buffer.Usage() == DataUsage::Read );

					return dd::ConstBuffer<T>( (const T*) buffer.Data(), buffer.Size() );
				}
			}

			throw std::exception( "No read buffer found for component. Check your requirements!" );
		}

		template <typename T>
		dd::Buffer<T> Write() const
		{
			for( const ComponentBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == T::Type )
				{
					DD_ASSERT( buffer.Usage() == DataUsage::Write );

					return dd::Buffer<T>( (T*) buffer.Data(), buffer.Size() );
				}
			}

			throw std::exception( "No write buffer found for component. Check your requirements!" );
		}

		void Commit();

	private:

		float m_delta;
		ddc::World& m_world;
		dd::Span<Entity> m_entities;
		std::vector<ComponentBuffer> m_buffers;
	};
}