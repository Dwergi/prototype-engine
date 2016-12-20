//
// DoubleBuffer.h - DoubleBuffer wraps double buffering of any arbitrary type - just call Swap to switch the read and write buffers.
// Copyright (C) Sebastian Nordgren 
// December 20th 2015
//

#include "PrecompiledHeader.h"
#include "DoubleBuffer.h"

namespace dd
{
	DoubleBufferBase::DoubleBufferBase( void* read, void* write, bool is_owner )
		: m_read( read ),
		m_write( write ),
		m_isOwner( is_owner )
	{

	}

	DoubleBufferBase::~DoubleBufferBase()
	{
		if( m_isOwner )
		{
			delete m_read;
			delete m_write;
		}
	}

	void DoubleBufferBase::Swap()
	{
		std::swap( m_read, m_write );
	}
}