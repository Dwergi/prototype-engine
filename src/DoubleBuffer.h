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

		DoubleBuffer( T* read, T* write, bool is_owner = true );
		~DoubleBuffer();

		DoubleBuffer( const DoubleBuffer<T>& ) = delete;
		DoubleBuffer( DoubleBuffer&& ) = delete;

		DoubleBuffer& operator=( const DoubleBuffer& ) = delete;
		DoubleBuffer& operator=( DoubleBuffer&& ) = delete;

		//
		// Duplicate the read buffer into the write buffer.
		//
		void Duplicate() const;

		//
		// Swap the read and write buffers.
		//
		void Swap();

		//
		// Get the read buffer.
		//
		const T& Read() const;

		//
		// Get the write buffer.
		//
		T& Write() const;

		DD_BASIC_TYPE( DoubleBuffer<T> )

	private:

		T* m_write { nullptr };
		T* m_read { nullptr };
		bool m_owner { true };
	};
}

#include "DoubleBuffer.inl"