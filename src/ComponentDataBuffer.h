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
	struct DataBuffer
	{
		DataBuffer( const ComponentDataBuffer& buffer, DataUsage usage ) :
			m_buffer( buffer ),
			m_usage( usage )
		{
			DD_ASSERT( buffer.Component() == T::Type );
			DD_ASSERT( buffer.Usage() == m_usage );
		}

		size_t Size() const { return m_buffer.Size(); }

		const T& Get( size_t index ) const
		{
			DD_ASSERT( index < m_buffer.Size() );
			DD_ASSERT( m_usage == DataUsage::Read || m_usage == DataUsage::ReadWrite );

			return *(reinterpret_cast<const T*>(m_buffer.Data()) + index);
		}

		T& Access( size_t index ) const
		{
			DD_ASSERT( index < m_buffer.Size() );
			DD_ASSERT( m_usage == DataUsage::Write || m_usage == DataUsage::ReadWrite );

			return *(reinterpret_cast<T*>(m_buffer.Data()) + index);
		}

	private:
		const ComponentDataBuffer& m_buffer;
		const DataUsage m_usage;
	};

	template <typename T>
	struct ReadBuffer : DataBuffer<T>
	{
		ReadBuffer( const ComponentDataBuffer& buffer ) :
			DataBuffer<T>( buffer, DataUsage::Read )
		{
		}

		ReadBuffer( const ReadBuffer& other ) :
			DataBuffer<T>( other.m_buffer, DataUsage::Read )
		{
		}
	};

	template <typename T>
	struct WriteBuffer : DataBuffer<T>
	{
		WriteBuffer( const ComponentDataBuffer& buffer ) :
			DataBuffer<T>( buffer, DataUsage::Write )
		{
		}

		WriteBuffer( const WriteBuffer& other ) :
			DataBuffer<T>( other.m_buffer, DataUsage::Write )
		{
		}
	};

	template <typename T>
	struct ReadWriteBuffer : DataBuffer<T>
	{
		ReadWriteBuffer( const ComponentDataBuffer& buffer ) :
			DataBuffer<T>( buffer, DataUsage::ReadWrite )

		{
		}

		ReadWriteBuffer( const ReadWriteBuffer& other ) :
			DataBuffer<T>( other.m_buffer, DataUsage::ReadWrite )
		{
		}
	};
}