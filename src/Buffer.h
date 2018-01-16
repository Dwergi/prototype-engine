//
// Buffer.h - A wrapper around a buffer. Just holds a pointer and a size. 
// Is stupid and won't clean up for you.
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#pragma once

namespace dd
{
	//
	// IBuffer - Buffer interface.
	//
	class IBuffer
	{
	public:

		virtual ~IBuffer();

		const void* GetVoid() const;
		int SizeBytes() const;

	protected:

		IBuffer( uint element_size );

		int m_count { 0 };
		const void* m_ptr { nullptr };

	private:

		uint m_elementSize { 0 };
	};

	//
	// ConstBuffer - const version of buffer.
	//
	template <typename T>
	class ConstBuffer : public IBuffer
	{
	public:

		ConstBuffer();
		ConstBuffer( const T* ptr, int count );
		ConstBuffer( const ConstBuffer<T>& other );
		ConstBuffer( ConstBuffer<T>&& other );
		virtual ~ConstBuffer();

		ConstBuffer<T>& operator=( const ConstBuffer<T>& other );
		ConstBuffer<T>& operator=( ConstBuffer<T>&& other );

		const T& operator[]( int idx ) const;

		void Set( const T* ptr, int count );
		const T* GetConst() const;

		const T* ReleaseConst();

		int Size() const;

		void Delete();

		bool operator==( const ConstBuffer<T>& other ) const;
		bool operator!=( const ConstBuffer<T>& other ) const;
	};

	//
	// Buffer - non-const version of buffer
	//
	template <typename T>
	class Buffer : public ConstBuffer<T>
	{
	public:

		Buffer();
		Buffer( T* ptr, int count );
		Buffer( const Buffer<T>& other );
		Buffer( Buffer<T>&& other );
		virtual ~Buffer();

		Buffer<T>& operator=( const Buffer<T>& other );
		Buffer<T>& operator=( Buffer<T>&& other );

		T& operator[]( int idx ) const;

		void Set( T* ptr, int count );
		T* Get() const;

		T* Release();

		bool operator==( const Buffer<T>& other ) const;
		bool operator!=( const Buffer<T>& other ) const;
	};

	#include "Buffer.inl"
}
