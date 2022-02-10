//
// Buffer.h - A wrapper around a buffer. Just holds a pointer and a size. 
// Is stupid and won't clean up for you.
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#pragma once

#include "DDAssert.h"

namespace dd
{
	//
	// IBuffer - Buffer interface.
	//
	struct IBuffer
	{
		virtual ~IBuffer();

		virtual const void* GetVoid() const = 0;
		uint64 SizeBytes() const;
		int SizeBytesInt() const;
		uint64 Size() const;
		int SizeInt() const;

	protected:

		IBuffer(uint element_size);

		uint64 m_count { 0 };
		uint64 m_elementSize { 0 };
	};

	//
	// ConstBuffer - const version of buffer.
	//
	template <typename T>
	struct ConstBuffer : IBuffer
	{
		ConstBuffer();
		ConstBuffer(const T* ptr, int count);
		ConstBuffer(const T* ptr, uint count);
		ConstBuffer(const T* ptr, uint64 count);

		explicit ConstBuffer(const std::vector<T>& vec);

		explicit ConstBuffer(const IBuffer& buffer);

		ConstBuffer(const ConstBuffer<T>& other);
		ConstBuffer(ConstBuffer<T>&& other);
		virtual ~ConstBuffer();

		ConstBuffer<T>& operator=(const ConstBuffer<T>& other);
		ConstBuffer<T>& operator=(ConstBuffer<T>&& other);

		const T& operator[](uint64 idx) const;

		bool IsValid() const { return m_ptr != nullptr; }

		void Set(const T* ptr, uint64 count);
		const T* Get() const;
		virtual const void* GetVoid() const override { return m_ptr; }

		const T* ReleaseConst();

		void Delete();

		bool operator==(const ConstBuffer<T>& other) const;
		bool operator!=(const ConstBuffer<T>& other) const;

		const T* begin() const { return Get(); }
		const T* end() const { return Get() + Size(); }

	protected:
		const T* m_ptr { nullptr };
	};

	//
	// Buffer - non-const version of buffer
	//
	template <typename T>
	struct Buffer : ConstBuffer<T>
	{
		Buffer();
		Buffer(T* ptr, int count);
		Buffer(T* ptr, uint count);
		Buffer(T* ptr, uint64 count);

		template <uint64 Size>
		explicit Buffer(T(arr)[Size]) : ConstBuffer(arr) {}

		explicit Buffer(const IBuffer& buffer);
		Buffer(const Buffer<T>& other);
		Buffer(Buffer<T>&& other);
		virtual ~Buffer();

		Buffer<T>& operator=(const Buffer<T>& other);
		Buffer<T>& operator=(Buffer<T>&& other);

		T& operator[](uint64 idx) const;

		void Set(T* ptr, int count);
		void Set(T* ptr, uint64 count);
		T* Access() const;

		void Fill(const T& value);

		T* Release();

		bool operator==(const Buffer<T>& other) const;
		bool operator!=(const Buffer<T>& other) const;

		T* begin() const { return Access(); }
		T* end() const { return Access() + Size(); }
	};

#include "Buffer.inl"
}
