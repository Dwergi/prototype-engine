#pragma once

#include "DataRequirement.h"
#include "EntityLayer.h"

namespace ddc
{
	struct ComponentDataBuffer
	{
		ComponentDataBuffer( dd::Span<Entity> entities, EntityLayer& space, const ComponentType& component, DataUsage usage, byte* storage );

		const ComponentType& Component() const { return m_component; }
		DataUsage Usage() const { return m_usage; }
		size_t Size() const { return m_count; }
		void* Data() const { return m_storage; }

		void Commit( dd::Span<Entity> entities, EntityLayer& layer );

	private:
		const ComponentType& m_component;
		byte* m_storage { nullptr };

		DataUsage m_usage { DataUsage::Invalid };
		size_t m_count { 0 };
	};

	template <typename T>
	struct ReadBuffer
	{
		ReadBuffer( const ComponentDataBuffer& buffer )
			: m_buffer( buffer )
		{
			DD_ASSERT( buffer.Usage() == DataUsage::Read );
			DD_ASSERT( buffer.Component() == T::Type );
		}

		ReadBuffer( const ReadBuffer& other ) :
			m_buffer( other.m_buffer )
		{
		}

		size_t Size() const { return m_buffer.Size(); }

		const T& Get( size_t index ) const
		{
			DD_ASSERT( index < m_buffer.Size() );

			return *(reinterpret_cast<T*>(m_buffer.Data()) + index);
		}

	private:
		const ComponentDataBuffer& m_buffer;
	};

	template <typename T>
	struct WriteBuffer
	{
		WriteBuffer( const ComponentDataBuffer& buffer )
			: m_buffer( buffer )
		{
			DD_ASSERT( buffer.Usage() == DataUsage::Write );
			DD_ASSERT( buffer.Component() == T::Type );
		}

		WriteBuffer( const WriteBuffer& other ) :
			m_buffer( other.m_buffer )
		{
		}

		size_t Size() const { return m_buffer.Size(); }

		T& Get( size_t index ) const
		{
			DD_ASSERT( index < m_buffer.Size() );

			return *(reinterpret_cast<T*>(m_buffer.Data()) + index);
		}

	private:
		const ComponentDataBuffer& m_buffer;
	};
}