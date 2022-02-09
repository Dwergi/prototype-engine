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
		explicit RingBuffer(uint64 size);
		RingBuffer(const RingBuffer& other);
		RingBuffer(RingBuffer&& other);
		~RingBuffer();

		RingBuffer& operator=(const RingBuffer& other);
		RingBuffer& operator=(RingBuffer&& other) noexcept;

		void Push(const T& item);
		void Push(T&& item);
		T& Advance();

		T Pop();

		uint64 Size() const;
		uint64 Capacity() const;

	private:
		static const uint64 DefaultSize { 8 };

		Buffer<T> m_storage;
		uint64 m_head { 0 };
		uint64 m_tail { 0 };
		uint64 m_size { 0 };

		void Grow();
		void Allocate(uint64 size);
	};
}

#include "RingBuffer.inl"