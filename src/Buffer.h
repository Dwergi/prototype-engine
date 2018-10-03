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
		ConstBuffer( const T* ptr, int count );
		ConstBuffer( const T* ptr, size_t count );

		template <size_t Size>
		explicit ConstBuffer( const T( arr )[Size] ) : IBuffer( sizeof( T ) )
		{
			m_ptr = arr;
			m_size = (int) Size 
		}

		explicit ConstBuffer( const IBuffer& buffer );
		ConstBuffer( const ConstBuffer<T>& other );
		ConstBuffer( ConstBuffer<T>&& other );
		virtual ~ConstBuffer();

		ConstBuffer<T>& operator=( const ConstBuffer<T>& other );
		ConstBuffer<T>& operator=( ConstBuffer<T>&& other );

		const T& operator[]( size_t idx ) const;

		bool IsValid() const { return m_ptr != nullptr; }

		void Set( const T* ptr, int count );
		const T* Get() const;
		virtual const void* GetVoid() const override { return m_ptr; }

		const T* ReleaseConst();

		int Size() const;

		void Delete();

		bool operator==( const ConstBuffer<T>& other ) const;
		bool operator!=( const ConstBuffer<T>& other ) const;

		const T* begin() const { return Get(); }
		const T* end() const { return Get() + Size(); }

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

		template <size_t Size>
		explicit Buffer( T( arr )[Size] ) : ConstBuffer( arr ) {}

		explicit Buffer( const IBuffer& buffer );
		Buffer( const Buffer<T>& other );
		Buffer( Buffer<T>&& other );
		virtual ~Buffer();

		Buffer<T>& operator=( const Buffer<T>& other );
		Buffer<T>& operator=( Buffer<T>&& other );

		T& operator[]( size_t idx ) const;

		void Set( T* ptr, int count );
		T* Access() const;

		T* Release();

		bool operator==( const Buffer<T>& other ) const;
		bool operator!=( const Buffer<T>& other ) const;

		T* begin() const { return Access(); }
		T* end() const { return Access() + Size(); }
	};

	#include "Buffer.inl"
}
