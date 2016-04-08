//
// DoubleBuffer.h - DoubleBuffer wraps double buffering of any arbitrary type - just call Swap to switch the read and write buffers.
// By default does not perform any copying, so make sure that you update everything in the write buffer or they'll be out of sync.
// Or call Duplicate to perform a copy.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	template <typename T>
	class DoubleBuffer
	{
	public:

		DoubleBuffer( T* read, T* write )
			: m_read( read ),
			m_write( write )
		{
		}

		~DoubleBuffer()
		{
		}

		void Swap()
		{
			std::swap( m_read, m_write );
		}

		//
		// Duplicate the read buffer into the write buffer.
		//
		void Duplicate() const
		{
			*m_write = *m_read;
		}

		const T& GetRead() const
		{
			return *m_read;
		}

		T& GetWrite() const
		{
			return *m_write;
		}

	private:

		T* m_write;
		T* m_read;
	};
}