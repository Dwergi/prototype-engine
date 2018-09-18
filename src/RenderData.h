//
// RenderData.h - Data used for renderer updates.
// Copyright (C) Sebastian Nordgren 
// August 26th 2018
//

#pragma once

#include "ComponentBuffer.h"
#include "ICamera.h"
#include "Span.h"
#include "Uniforms.h"

namespace ddc
{
	struct DataRequest;
	struct Entity;
	struct World;
}

namespace ddr
{
	template <typename T>
	struct RenderBuffer
	{
		RenderBuffer( const ddc::ComponentBuffer& buffer ) :
			m_buffer( buffer )
		{
			DD_ASSERT( buffer.Component() == T::Type );
			DD_ASSERT( buffer.Usage() == ddc::DataUsage::Read );
		}

		size_t Size() const { return m_buffer.Size(); }

		const T& Get( size_t index ) const
		{
			DD_ASSERT( index < m_buffer.Size() );

			return *(Data() + index);
		}

		const T& operator[]( size_t index ) const
		{
			return Get( index );
		}

		const T* begin() const { return Data(); }
		const T* end() const { return Data() + Size(); }

	protected:
		const ddc::ComponentBuffer& m_buffer;

		const T* Data() const
		{
			return reinterpret_cast<const T*>(m_buffer.Data());
		}
	};

	struct RenderData
	{
	public:
		RenderData( ddc::World& world, const ddr::ICamera& camera, ddr::UniformStorage& uniforms,
			dd::Span<ddc::Entity> entities, const dd::IArray<const ddc::DataRequest*>& requirements );

		ddc::World& World() const { return m_world; }
		const ddr::ICamera& Camera() const { return m_camera; }

		ddr::UniformStorage& Uniforms() const { return m_uniforms; }

		const dd::Span<ddc::Entity> Entities() const { return m_entities; }
		size_t Size() const { return m_entities.Size(); }

		template <typename T>
		ddr::RenderBuffer<T> Get() const
		{
			for( const ddc::ComponentBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == T::Type )
				{
					return ddr::RenderBuffer<T>( buffer );
				}
			}

			throw std::exception( "No read buffer found for component. Check your requirements!" );
		}

	private:
		ddc::World& m_world;
		const ddr::ICamera& m_camera;
		ddr::UniformStorage& m_uniforms;
		dd::Span<ddc::Entity> m_entities;

		std::vector<ddc::ComponentBuffer> m_buffers;
	};
}