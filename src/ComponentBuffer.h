//
// ComponentBuffer.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "DataRequest.h"

namespace ddc
{
	struct ComponentBuffer
	{
		ComponentBuffer();
		explicit ComponentBuffer(DataRequest* request);
		ComponentBuffer(ComponentBuffer&& other);

		const dd::TypeInfo& Component() const { return m_request->Component(); }
		DataUsage Usage() const { return m_request->Usage(); }

		size_t Size() const { return m_count; }
		byte* Data() const { return m_storage; }

		bool Optional() const { return m_request->Optional(); }
		bool Has( size_t i ) const { return !Optional() || m_exists.at( i ); }
		
	private:

		friend struct UpdateDataBuffer;

		void Fill(const std::vector<Entity>& entities);
		void Commit(const std::vector<ddc::Entity>& entities);
		
		DataRequest* m_request { nullptr };
		size_t m_count { 0 };

		std::vector<bool> m_exists;
		byte* m_storage { nullptr };
	};

	template <typename TComponent>
	struct ReadView
	{
		ReadView( const ComponentBuffer& buffer ) :
			m_buffer( buffer ),
			m_data( reinterpret_cast<const TComponent*>(m_buffer.Data()) )
		{
			DD_ASSERT( m_buffer.Usage() == DataUsage::Read );
		}

		bool Optional() const { return m_buffer.Optional(); }
		bool Has( size_t i ) const { return m_buffer.Has( i ); }
		size_t Size() const { return m_buffer.Size(); }

		const TComponent* Get( size_t i ) const
		{
			DD_ASSERT( i < Size() );
			if( Has( i ) )
			{
				return At( i );
			}
			return nullptr;
		}

		const TComponent& operator[]( size_t i ) const
		{
			DD_ASSERT( Has( i ) );
			return *At( i );
		}

		const TComponent* begin() const
		{
			DD_ASSERT( !Optional(), "Cannot iterate over optional buffers!" );
			return At( 0 );
		}

		const TComponent* end() const
		{
			DD_ASSERT( !Optional(), "Cannot iterate over optional buffers!" );
			return At( Size() );
		}

	private:

		const ComponentBuffer& m_buffer;
		const TComponent* const m_data;

		const TComponent* At( size_t i ) const
		{
			return m_data + i;
		}
	};

	template <typename TComponent>
	struct WriteView
	{
		WriteView( const ComponentBuffer& buffer ) : 
			m_buffer( buffer ),
			m_data( reinterpret_cast<TComponent*>(m_buffer.Data()) )
		{
			DD_ASSERT( m_buffer.Usage() == DataUsage::Write );
		}

		bool Optional() const { return m_buffer.Optional(); }
		bool Has( size_t i ) const { return m_buffer.Has( i ); }
		size_t Size() const { return m_buffer.Size(); }

		TComponent* Access( size_t i ) const
		{
			DD_ASSERT( i < Size() );
			if( Has( i ) )
			{
				return At( i );
			}
			return nullptr;
		}

		TComponent& operator[]( size_t i ) const
		{
			DD_ASSERT( Has( i ) );
			return *At( i );
		}

		TComponent* begin() const
		{
			DD_ASSERT( !Optional(), "Cannot iterate over optional buffers!" );
			return At( 0 );
		}

		TComponent* end() const
		{
			DD_ASSERT( !Optional(), "Cannot iterate over optional buffers!" );
			return At( Size() );
		}

	private:

		const ComponentBuffer& m_buffer;
		TComponent* const m_data;

		TComponent* At( size_t i ) const
		{
			return m_data + i;
		}
	};
}