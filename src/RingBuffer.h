//
// RingBuffer.h - A ring buffer.
// Copyright (C) Sebastian Nordgren 
// December 17th 2015
//

#pragma once

namespace dd
{
	template <typename T>
	struct RingBuffer
	{
	public:

		RingBuffer();
		explicit RingBuffer( uint size );
		~RingBuffer();

		void Push( const T& item );
		void Push( T&& item );

		T Pop();

		uint Size() const;
		uint Capacity() const;

	private:
		const uint DefaultSize { 8 };

		Buffer<T> m_storage;
		uint m_head { 0 };
		uint m_tail { 0 };
		uint m_size { 0 };

		void Grow();
		void Allocate( uint size );
	};
}

#include "RingBuffer.inl"