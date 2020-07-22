//
// Array.h - An in-place, fixed-size array.
// Copyright (C) Sebastian Nordgren 
// September 3rd 2015
//

#pragma once

#include "Buffer.h"

namespace dd
{
	template <typename T>
	class IArray
	{
	public:
		int Capacity() const { return m_data.Size(); }
		int Size() const { return m_size; }

		virtual ~IArray();

		IArray<T>& operator=(const IArray<T>& other);
		IArray<T>& operator=(IArray<T>&& other);

		T& Push(const T& value);
		T& Push(T&& value);

		T& Add(const T& value) { return Push(value); }
		T& Add(T&& value) { return Push(std::move(value)); }

		void PushAll(const IArray<T>& other);
		void PushAll(IArray<T>&& other);

		void Fill(const T& value);

		void RemoveAt(int index);
		void Remove(const T& value);

		T Pop();
		void Clear();

		const T& operator[](size_t index) const;
		bool Contains(const T& val);
		int IndexOf(const T& val);

		T& Last() const;
		T& First() const;

		T* Data() { return m_data; }
		void SetSize(int size) { m_size = size; }

		T* begin() const { return m_data.begin(); }
		T* end() const { return m_data.begin() + Size(); }

	protected:

		int m_size { 0 };
		Buffer<T> m_data;

		IArray(T* buffer, int capacity);
	};

	//
	// Array is, as the name implies, a static array that is allocated on the stack. Useful for temporary passing around of values. 
	//
	template <typename T, int MaxCapacity>
	class Array
		: public IArray<T>
	{
	public:

		Array();
		Array(const Array<T, MaxCapacity>& other);
		Array(Array<T, MaxCapacity>&& other) noexcept;

		Array<T, MaxCapacity>& operator=(const Array<T, MaxCapacity>& other);
		Array<T, MaxCapacity>& operator=(Array<T, MaxCapacity>&& other);

		virtual ~Array();

	private:

		T m_inline[MaxCapacity];
	};

	#include "Array.inl"
}
