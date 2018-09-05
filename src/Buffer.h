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

		virtual const void* GetVoid() const = 0;
		int SizeBytes() const;

	protected:

		IBuffer( uint element_size );

		int m_count { 0 };
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
		ConstBuffer( const IBuffer& buffer );
		ConstBuffer( const T* ptr, int count );
		ConstBuffer( const T* ptr, size_t count );
		ConstBuffer( const ConstBuffer<T>& other );
		ConstBuffer( ConstBuffer<T>&& other );
		virtual ~ConstBuffer();

		ConstBuffer<T>& operator=( const ConstBuffer<T>& other );
		ConstBuffer<T>& operator=( ConstBuffer<T>&& other );

		const T& operator[]( size_t idx ) const;

		bool IsValid() const { return m_ptr != nullptr; }

		void Set( const T* ptr, int count );
		const T* GetConst() const;
		virtual const void* GetVoid() const override { return m_ptr; }

		const T* ReleaseConst();

		int Size() const;

		void Delete();

		bool operator==( const ConstBuffer<T>& other ) const;
		bool operator!=( const ConstBuffer<T>& other ) const;

		const T* begin() const { return GetConst(); }
		const T* end() const { return GetConst() + Size(); }

	protected:
		const T* m_ptr { nullptr };
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
		Buffer( T* ptr, size_t count );
		Buffer( const Buffer<T>& other );
		Buffer( Buffer<T>&& other );
		virtual ~Buffer();

		Buffer<T>& operator=( const Buffer<T>& other );
		Buffer<T>& operator=( Buffer<T>&& other );

		T& operator[]( size_t idx ) const;

		void Set( T* ptr, int count );
		T* Get() const;

		T* Release();

		bool operator==( const Buffer<T>& other ) const;
		bool operator!=( const Buffer<T>& other ) const;

		T* begin() const { return Get(); }
		T* end() const { return Get() + Size(); }
	};

	#include "Buffer.inl"
}
