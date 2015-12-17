//
// RingBuffer.h - A ring buffer.
// Copyright (C) Sebastian Nordgren 
// December 17th 2015
//

#pragma once

namespace dd
{
	template <typename T>
	class RingBuffer
	{
	public:

		RingBuffer();
		RingBuffer( uint size );
		~RingBuffer();

		void Push( const T& item );
		void Push( T&& item );

		T Pop();

		uint Size() const;
		uint Capacity() const;

	private:
		const uint DefaultSize = 8;

		Buffer<T> m_storage;
		int m_head;
		int m_tail;

		void Grow();
		void Allocate( uint size );
	};
}

#include "RingBuffer.inl"