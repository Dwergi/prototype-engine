#pragma once

#include "DataRequirement.h"
#include "World.h"

namespace ddc
{
	struct ComponentBuffer
	{
		ComponentBuffer( dd::Span<Entity> entities, const World& world, const ComponentType& component, DataUsage usage, byte* storage );

		const ComponentType& Component() const { return m_component; }
		DataUsage Usage() const { return m_usage; }
		size_t Size() const { return m_count; }
		byte* Data() const { return m_storage; }
		
	private:
		const ComponentType& m_component;
		byte* m_storage { nullptr };

		DataUsage m_usage { DataUsage::Invalid };
		size_t m_count { 0 };
	};

	/*template <typename T>
	struct DataBuffer
	{
		DataBuffer( const ComponentBuffer& buffer, DataUsage usage ) :
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
			DD_ASSERT( m_usage == DataUsage::Read );

			return *(ConstData() + index);
		}

		T& Access( size_t index ) const
		{
			DD_ASSERT( index < m_buffer.Size() );
			DD_ASSERT( m_usage == DataUsage::Write );

			return *(Data() + index);
		}

		const T& operator[]( size_t index ) const
		{
			return Get( index );
		}

	protected:
		const ComponentBuffer& m_buffer;
		const DataUsage m_usage;

		const T* ConstData() const 
		{
			return reinterpret_cast<const T*>(m_buffer.Data());
		}

		T* Data() const
		{ 
			return reinterpret_cast<T*>(m_buffer.Data());
		}
	};

	template <typename T>
	struct ReadBuffer : DataBuffer<T>
	{
		ReadBuffer( const ComponentBuffer& buffer ) :
			DataBuffer<T>( buffer, DataUsage::Read )
		{
		}

		ReadBuffer( const ReadBuffer& other ) :
			DataBuffer<T>( other.m_buffer, DataUsage::Read )
		{
		}

		const T* begin() const { return ConstData(); }
		const T* end() const { return ConstData() + Size(); }
	};

	template <typename T>
	struct WriteBuffer : DataBuffer<T>
	{
		WriteBuffer( const ComponentBuffer& buffer ) :
			DataBuffer<T>( buffer, DataUsage::Write )
		{
		}

		WriteBuffer( const WriteBuffer& other ) :
			DataBuffer<T>( other.m_buffer, DataUsage::Write )
		{
		}

		T& operator[]( size_t index )
		{
			return Access( index );
		}

		T* begin() const { return Data(); }
		T* end() const { return Data() + Size(); }
	};*/
}