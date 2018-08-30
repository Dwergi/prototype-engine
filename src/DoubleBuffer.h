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
	class IDoubleBuffer
	{
	public:

		virtual ~IDoubleBuffer();

		IDoubleBuffer( const IDoubleBuffer& ) = delete;
		IDoubleBuffer( IDoubleBuffer&& ) = delete;

		IDoubleBuffer& operator=( const IDoubleBuffer& ) = delete;
		IDoubleBuffer& operator=( IDoubleBuffer&& ) = delete;

		//
		// Swap the read and write buffers.
		//
		void Swap();

		virtual void Duplicate() const = 0;

		void* GetWriteVoid() const { return m_write; }

	protected:

		void* m_write;
		void* m_read;

		bool m_isOwner;

		IDoubleBuffer( void* read, void* write, bool is_owner );
	};

	template <typename T>
	class DoubleBuffer : public IDoubleBuffer
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
		// Get the read buffer.
		//
		const T& Read() const;

		//
		// Get the write buffer.
		//
		T& Write() const;

		DD_BASIC_TYPE( DoubleBuffer<T> )
	};
}

#include "DoubleBuffer.inl"