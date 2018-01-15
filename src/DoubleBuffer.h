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
	class DoubleBufferBase
	{
	public:

		virtual ~DoubleBufferBase();

		DoubleBufferBase( const DoubleBufferBase& ) = delete;
		DoubleBufferBase( DoubleBufferBase&& ) = delete;

		DoubleBufferBase& operator=( const DoubleBufferBase& ) = delete;
		DoubleBufferBase& operator=( DoubleBufferBase&& ) = delete;

		//
		// Swap the read and write buffers.
		//
		void Swap();

		virtual void Clear() const = 0;
		virtual void Duplicate() const = 0;

		void* GetWriteVoid() const { return m_write; }

	protected:

		void* m_write;
		void* m_read;

		bool m_isOwner;

		DoubleBufferBase( void* read, void* write, bool is_owner );
	};

	template <typename T>
	class DoubleBuffer : public DoubleBufferBase
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
		virtual void Duplicate() const override;

		//
		// Clear the contents of both the read and write buffers.
		//
		virtual void Clear() const override;

		//
		// Get the read buffer.
		//
		const T& GetRead() const;

		//
		// Get the write buffer.
		//
		T& GetWrite() const;

		BASIC_TYPE( DoubleBuffer<T> )
	};
}

#include "DoubleBuffer.inl"