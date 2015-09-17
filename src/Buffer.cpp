//
// Buffer.cpp - A wrapper around a buffer. Just holds a pointer and a size.
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#include "PrecompiledHeader.h"
#include "Buffer.h"

namespace dd
{
	Buffer::Buffer()
		: m_ptr( nullptr ),
		m_size( 0 )
	{

	}

	Buffer::Buffer( Buffer&& other )
	{
		Set( other.m_ptr, other.m_size );

		other.m_ptr = nullptr;
		other.m_size = 0;
	}

	Buffer::~Buffer()
	{
		Set( nullptr, 0 );
	}

	void Buffer::Set( const void* ptr, uint size )
	{
		if( m_ptr != nullptr )
		{
			delete m_ptr;
		}

		m_ptr = ptr;
		m_size = size;
	}

	const void* Buffer::Get() const
	{
		return m_ptr;
	}

	void Buffer::Resize( uint size )
	{
		if( size == 0 )
		{
			Set( nullptr, 0 );
			return;
		}

		void* new_ptr = new char[ size ];
		memcpy( new_ptr, m_ptr, m_size );

		if( m_ptr != nullptr )
		{
			delete[] m_ptr;
		}

		m_ptr = new_ptr;
		m_size = size;
	}

	uint Buffer::Size() const
	{
		return m_size;
	}
}