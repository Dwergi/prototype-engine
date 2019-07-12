//
// Array.h - An in-place, fixed-size array.
// Copyright (C) Sebastian Nordgren 
// September 3rd 2015
//

#pragma once

namespace dd
{
	template <typename T>
	class IArray
	{
	public:
		int Capacity() const { return m_data.Size(); }
		int Size() const { return m_size; }

		virtual ~IArray();
		IArray<T>& operator=( const IArray<T>& other );

		void Push( const T& value );
		void Add( const T& value );
		void PushAll( const IArray<T>& other );

		void RemoveAt( int index );
		void Remove( const T& value );

		T Pop();
		void Clear();

		const T& operator[]( size_t index ) const;
		bool Contains( const T& val );
		int IndexOf( const T& val );

		T& Last() const;
		T& First() const;

		T* Data() { return m_data; }
		void SetSize( int size ) { m_size = size; }

		T* begin() const { return m_data.begin(); }
		T* end() const { return m_data.end(); }

	protected:

		int m_size;
		Buffer<T> m_data;

		IArray( T* buffer, int capacity );
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
		Array( const Array<T, MaxCapacity>& other );

		virtual ~Array();

	private:

		T m_inline[ MaxCapacity ];
	};
}

#include "Array.inl"