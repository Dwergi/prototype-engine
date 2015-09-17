//
// Buffer.h - A wrapper around a buffer. Just holds a pointer and a size.
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#pragma once

namespace dd
{
	class Buffer
	{
	public:

		Buffer();
		Buffer( Buffer&& other );
		~Buffer();

		void Set( const void* ptr, uint size );
		const void* Get() const;

		void Resize( uint size );
		uint Size() const;

	private:

		const void* m_ptr;
		uint m_size;

		Buffer( const Buffer& other ) {} // copying disallowed
	};
}
