//
// String_dd.h - String implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Iteration.h"

namespace dd
{
	class String
	{
	protected:
		void Initialize();

	public:

		String();
		virtual ~String();

		String& operator=( const char* other );
		String& operator=( const String& other );
		bool operator==( const char* other ) const;
		bool operator==( const String& other ) const;
		String& operator+=( const String& other );
		String& operator+=( const char* other );

		int Find( const String& other ) const;
		String& Substring( uint start, uint end );

		void Clear() { m_length = 0; }

		const char& operator[]( uint index ) { ASSERT( index >= 0 && index < m_length ); return m_buffer[ index ]; }
		const char* c_str() const;

		uint Length() const { return m_length; }
		bool IsEmpty() const { return m_length == 0; }

		static void RegisterMembers() { }

		DEFINE_ITERATORS( char, m_buffer, m_length );

	protected:

		uint m_length;

		uint m_capacity;
		char* m_buffer;
		
		uint m_stackCapacity;
		char* m_stackBuffer;

		void Resize( uint length );
		void SetString( const char* data, uint length );
		void Concatenate( const char* other, uint length );
		bool Equals( const char* other, uint length ) const;
	};

	//
	// An in-place implementation of string which always allocates a fixed size buffer. 
	//
	template< int Size = 32 >
	class InplaceString
		: public String
	{
	public:

		InplaceString()
		{
			Initialize();
		}

		InplaceString( const char* other )
		{
			Initialize();
			SetString( other, (uint) strlen( other ) );
		}

		InplaceString( const String& other )
		{
			Initialize();
			SetString( other.c_str(), other.Length() );
		}

		virtual ~InplaceString()
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

	typedef InplaceString<8> String8;
	typedef InplaceString<16> String16;
	typedef InplaceString<32> String32;
	typedef InplaceString<64> String64;
	typedef InplaceString<128> String128;
	typedef InplaceString<256> String256;
}
