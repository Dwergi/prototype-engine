//
// Buffer.h - A wrapper around a buffer. Just holds a pointer and a size. 
// Is stupid and won't clean up for you.
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#pragma once

namespace dd
{
	template<typename T>
	class Buffer
	{
	public:

		Buffer();
		Buffer( T* ptr, uint size );
		Buffer( const Buffer<T>& other );
		Buffer( Buffer<T>&& other );
		~Buffer();

		operator T*() const;
		T& operator[]( uint idx ) const;

		Buffer<T>& operator=( const Buffer<T>& other );
		Buffer<T>& operator=( Buffer<T>&& other );
		void Set( T* ptr, uint size );

		T* Get() const;
		T* Release();
		uint Size() const;

		bool operator==( const Buffer<T>& other ) const;
		bool operator!=( const Buffer<T>& other ) const;

	private:

		T* m_ptr;
		uint m_size;
	};

	#include "Buffer.inl"
}
