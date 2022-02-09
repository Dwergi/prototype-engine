//
// Buffer.cpp - A wrapper around a buffer. Just holds a pointer and a size.
// Copyright (C) Sebastian Nordgren 
// January 6th 2018
//

#include "PCH.h"
#include "Buffer.h"

namespace dd
{
	IBuffer::IBuffer(uint element_size) :
		m_elementSize(element_size)
	{

	}

	IBuffer::~IBuffer()
	{
		m_count = 0;
		m_elementSize = 0;
	}

	uint64 IBuffer::Size() const
	{
		return m_count;
	}

	int IBuffer::SizeInt() const
	{
		DD_ASSERT(m_count <= std::numeric_limits<int>::max());

		return (int) m_count;
	}

	uint64 IBuffer::SizeBytes() const
	{
		return m_count * m_elementSize;
	}

	int IBuffer::SizeBytesInt() const
	{
		uint64 byte_size = m_count * m_elementSize;
		DD_ASSERT(byte_size <= std::numeric_limits<int>::max());

		return (int) byte_size;

	}
}