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
	private:

		// disable
		String( const String& other ) {}

	protected:

		String( char* stackBuffer, uint stackCapacity );

	public:

		virtual ~String();

		String& operator=( const char* other );
		String& operator=( const String& other );
		bool operator==( const char* other ) const;
		bool operator==( const String& other ) const;
		String& operator+=( const String& other );
		String& operator+=( const char* other );
		String& operator+=( char other );

		int Find( const String& other ) const;
		String& Substring( uint start, uint end );

		void Clear() { m_length = 0; }

		const char& operator[]( uint index ) { ASSERT( index >= 0 && index < m_length ); return m_buffer[ index ]; }
		const char* c_str() const;

		uint Length() const { return m_length; }
		bool IsEmpty() const { return m_length == 0; }
		void ShrinkToFit();

		bool StartsWith( const char* other ) const;
		bool StartsWith( const String& other ) const;

		static void RegisterMembers() { }

		DEFINE_ITERATORS( const char, m_buffer, m_length );

	protected:

		Buffer<char> m_stack;
		Buffer<char> m_buffer;
		uint m_length;
		
		void Resize( uint length );
		void SetString( const char* data, uint length );
		void Concatenate( const char* other, uint length );
		bool Equals( const char* other, uint length ) const;

		void NullTerminate();
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
			: String( m_stackData, Size )
		{
			m_stackData[ 0 ] = '\0';
		}

		InplaceString( const char* other )
			: String( m_stackData, Size )
		{
			SetString( other, (uint) strlen( other ) );
		}

		InplaceString( const String& other )
			: String( m_stackData, Size )
		{
			SetString( other.c_str(), other.Length() );
		}

		InplaceString( const dd::InplaceString<Size>& other )
			: String( m_stackData, Size )
		{
			SetString( other.c_str(), other.Length() );
		}

		template <int OtherSize>
		InplaceString( const InplaceString<OtherSize>& other )
			: String( m_stackData, Size )
		{
			SetString( other.c_str(), other.Length() );
		}

		virtual ~InplaceString()
		{

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
