//
// Span.h - A slice of a container.
// Copyright (C) Sebastian Nordgren 
// June 13th 2018
//

#pragma once

#include <vector>

namespace dd
{
	template <typename T>
	struct Span
	{
		Span( const std::vector<T>& container, size_t count = ~0, size_t offset = 0 )
		{
			if( container.empty() )
			{
				return;
			}

			if( count != ~0 )
			{
				m_size = count;
			}
			else
			{
				m_size = container.size();
			}

			m_offset = offset;

			const T* begin = &(*container.begin());
			m_begin = begin + m_offset;

		}

		Span( T* ptr, size_t count, size_t offset = 0 )
		{
			if( count == 0 )
			{
				return;
			}

			m_size = count;
			m_offset = offset;

			m_begin = ptr + offset;
		}

		template <size_t Size>
		Span( T( &arr )[ Size ], size_t count = Size, size_t offset = 0 )
		{
			m_size = count;
			m_offset = offset;

			m_begin = arr + m_offset;
		}

		void operator=( const dd::Span<T>& other )
		{
			m_size = other.m_size;
			m_offset = other.m_offset;

			m_begin = other.m_begin;
		}

		size_t Offset() const
		{
			return m_offset;
		}

		size_t Size() const
		{
			return m_size;
		}

		const T& operator[]( size_t index ) const
		{
			DD_ASSERT( index < m_size );

			return *(m_begin + index);
		}

		const T* begin() const
		{
			return m_begin;
		}

		const T* end() const
		{
			return m_begin + m_offset + m_size;
		}

		const T* m_begin { nullptr };

		size_t m_size { 0 };
		size_t m_offset { 0 };
	};
}