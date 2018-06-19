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
		Span( const std::vector<T>& container, size_t count, size_t offset = 0 )
		{
			const T* begin = &(*container.begin());
			m_begin = begin + offset;
			m_end = begin + offset + count;

			m_size = count;
			m_offset = offset;
		}

		Span( T* ptr, size_t count, size_t offset = 0 )
		{
			m_begin = ptr + offset;
			m_end = ptr + offset + count;

			m_size = count;
			m_offset = offset;
		}

		template <size_t Size>
		Span( T( &arr )[ Size ], size_t count = Size, size_t offset = 0 )
		{
			m_begin = arr + offset;
			m_end = arr + offset + count;

			m_size = count;
			m_offset = offset;
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
			return m_end;
		}

		const T* m_begin;
		const T* m_end;

		size_t m_size { 0 };
		size_t m_offset { 0 };
	};
}