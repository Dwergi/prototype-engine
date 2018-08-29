//
// RenderData.h - Data used for renderer updates.
// Copyright (C) Sebastian Nordgren 
// August 26th 2018
//

#pragma once

#include "ComponentDataBuffer.h"
#include "Span.h"

namespace ddc
{
	struct DataRequirement;
	struct Entity;
	struct World;
}

namespace ddr
{
	class ICamera;
	struct UniformStorage;

	struct RenderData
	{
	public:
		RenderData( ddc::World& world, dd::Span<ddc::Entity> entities, const dd::IArray<const ddc::DataRequirement*>& requirements,
			ddr::ICamera& camera, ddr::UniformStorage& uniforms );

		ddc::World& World() const { return m_world; }
		ddr::ICamera& Camera() const { return m_camera; }
		ddr::UniformStorage& Uniforms() const { return m_uniforms; }

		const dd::Span<ddc::Entity> Entities() const { return m_entities; }
		size_t Size() const { return m_entities.Size(); }

		template <typename T>
		ddc::ReadBuffer<T> Read() const
		{
			for( const ddc::ComponentDataBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == T::Type && 
					buffer.Usage() == ddc::DataUsage::Read )
				{
					return ddc::ReadBuffer<T>( buffer );
				}
			}

			throw std::exception( "No read buffer found for component. Check your requirements!" );
		}

		template <typename T>
		ddc::WriteBuffer<T> Write() const
		{
			for( const ddc::ComponentDataBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == T::Type && 
					buffer.Usage() == ddc::DataUsage::Write )
				{
					return ddc::WriteBuffer<T>( buffer );
				}
			}

			throw std::exception( "No read buffer found for component. Check your requirements!" );
		}

	private:
		ddc::World& m_world;
		ddr::ICamera& m_camera;
		ddr::UniformStorage& m_uniforms;
		dd::Span<ddc::Entity> m_entities;

		std::vector<ddc::ComponentDataBuffer> m_buffers;
	};
}