//
// RenderData.h - Data used for renderer updates.
// Copyright (C) Sebastian Nordgren 
// August 26th 2018
//

#pragma once

#include "CommandBuffer.h"
#include "ComponentBuffer.h"
#include "ICamera.h"
#include "Span.h"
#include "Uniforms.h"

namespace ddc
{
	struct DataRequest;
	struct Entity;
	struct EntityLayer;
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
		RenderData();
		RenderData(RenderData&& data);

		const ddc::EntityLayer& EntityLayer() const { return *m_layer; }
		ddc::EntityLayer& EntityLayer() { return *m_layer; }

		const ddr::ICamera& Camera() const { return *m_camera; }

		ddr::UniformStorage& Uniforms() const { return *m_uniforms; }

		float Delta() const { return m_delta; }

		const std::vector<ddc::Entity>& Entities() const { return m_entities; }
		size_t Size() const { return m_entities.size(); }

		template <typename T>
		ddc::ReadView<T> Get() const
		{
			const dd::TypeInfo* type = DD_FIND_TYPE( T );

			for( const ddc::ComponentBuffer& buffer : m_buffers )
			{
				if( &buffer.Component() == type )
				{
					DD_ASSERT( buffer.Usage() == ddc::DataUsage::Read );
					return ddc::ReadView<T>( buffer );
				}
			}
			throw std::exception( "No read buffer found for component. Check your requests!" );
		}

	private:
		friend struct RenderManager;

		ddc::EntityLayer* m_layer;
		const ddr::ICamera* m_camera;
		ddr::UniformStorage* m_uniforms;
		float m_delta { 0 };

		std::vector<ddc::Entity> m_entities;
		std::vector<ddc::ComponentBuffer> m_buffers;

		void Fill(ddc::EntityLayer& layer, const ddr::ICamera& camera, ddr::UniformStorage& uniforms,
			std::vector<ddc::Entity>&& entities, const dd::IArray<ddc::DataRequest*>& requirements, float delta_t);
	};
}