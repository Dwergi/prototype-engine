//
// Buffer.cpp - A wrapper around a buffer. Just holds a pointer and a size.
// Copyright (C) Sebastian Nordgren 
// January 6th 2018
//

#include "PCH.h"
#include "Buffer.h"

namespace dd
{
	IBuffer::IBuffer( uint element_size ) :
		m_elementSize( element_size )
	{

	}

	IBuffer::~IBuffer()
	{
		m_count = 0;
		m_elementSize = 0;
	}

	int IBuffer::SizeBytes() const
	{
		return m_count * m_elementSize;
	}
}