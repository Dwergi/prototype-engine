//
// Array.h - An in-place, fixed-size array.
// Copyright (C) Sebastian Nordgren 
// September 3rd 2015
//

#pragma once

namespace dd
{
	template <typename T>
	class ArrayBase
	{
	public:
		uint Capacity() const { return m_capacity; }
		uint Size() const { return m_size; }

		virtual ~ArrayBase();
		ArrayBase<T>& operator=( const ArrayBase<T>& other );

		void Push( const T& value );
		void PushAll( const ArrayBase<T>& other );

		void RemoveAt( uint index );
		void Remove( const T& value );

		T Pop();
		void Clear();

		const T& operator[]( uint index ) const;
		bool Contains( const T& val );
		int IndexOf( const T& val );

		T* Data() { return m_data; }
		void SetSize( uint size ) { m_size = size; }

		DEFINE_ITERATORS( T, m_data, m_size )

	protected:

		uint m_capacity;
		uint m_size;
		T* m_data;

		ArrayBase( T* buffer, uint capacity );
	};

	//
	// Array is, as the name implies, a static array that is allocated on the stack. Useful for temporary passing around of values. 
	//
	template <typename T, uint MaxCapacity>
	class Array 
		: public ArrayBase<T>
	{
	public:

		Array();
		Array( const Array<T, MaxCapacity>& other );

		virtual ~Array();

	private:

		T m_buffer[ MaxCapacity ];

		static_assert( sizeof( T ) * MaxCapacity < 16 * 1024, "Used more than 16K of stack size!" );
	};
}

#include "Array.inl"