//
// String_dd.h - String implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Iteration.h"

namespace dd
{
	class StringBase
	{
	protected:
		void Initialize();

		typedef unsigned int uint;

	protected:
		StringBase();

	public:

		virtual ~StringBase();

		StringBase& operator=( const char* other );
		StringBase& operator=( const StringBase& other );
		bool operator==( const char* other ) const;
		bool operator==( const StringBase& other ) const;
		StringBase& operator+=( const StringBase& other );

		int Find( const StringBase& other ) const;
		StringBase& Substring( uint start, uint end );

		void Clear() { m_length = 0; }

		const char& operator[]( uint index ) { ASSERT( index >= 0 && index < m_length ); return m_buffer[ index ]; }
		const char* c_str() const;

		uint Length() const { return m_length; }
		bool IsEmpty() const { return m_length == 0; }

		DEFINE_ITERATORS( char, m_buffer, m_length );

	protected:

		uint m_length;

		uint m_capacity;
		char* m_buffer;
		
		uint m_stackCapacity;
		char* m_stackBuffer;

		void Resize( uint length );
		void SetString( const char* data, uint length );
		bool Equals( const char* other, uint length ) const;
	};

	//
	// An in-place implementation of string which always allocates a fixed size buffer. 
	//
	template< int Size = 32 >
	class StackString
		: public StringBase
	{
	public:

		StackString()
		{
			Initialize();
		}

		StackString( const char* other )
		{
			Initialize();
			SetString( other, (uint) strlen( other ) );
		}

		StackString( const StringBase& other )
		{
			Initialize();
			SetString( other.c_str(), other.Length() );
		}

		virtual ~StackString()
		{

		}

	protected:

		void Initialize()
		{
			m_stackBuffer = m_stackData;
			m_stackCapacity = Size;

			m_capacity = m_stackCapacity;
			m_buffer = m_stackBuffer;
		}

	private:

		char m_stackData[ Size ];
	};

	typedef StackString<32> String;
	typedef StackString<16> String16;
	typedef StackString<32> String32;
	typedef StackString<64> String64;
	typedef StackString<128> String128;
	typedef StackString<256> String256;
}