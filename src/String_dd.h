//
// String_dd.h - String implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Iteration.h"

namespace dd
{
	class TypeInfo;

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

		String& operator+=( const String& other );
		String& operator+=( const char* other );
		String& operator+=( char other );

		bool operator==( const char* other ) const;
		bool operator==( const String& other ) const;

		bool operator!=( const char* other ) const;
		bool operator!=( const String& other ) const;

		bool EqualsCaseless( const char* other ) const;
		bool EqualsCaseless( const String& other ) const;

		int Find( const char* other, uint offset = 0 ) const;
		int Find( const String& other, uint offset = 0 ) const;

		void ReplaceAll( char src, char target );

		void Clear() { m_length = 0; }

		const char& operator[]( uint index ) const { DD_ASSERT( index >= 0 && index < m_length ); return m_buffer[ index ]; }
		char& operator[]( uint index ) { DD_ASSERT( index >= 0 && index < m_length ); return m_buffer[index]; }
		const char* c_str() const;

		uint Length() const { return m_length; }
		bool IsEmpty() const { return m_length == 0; }
		void ShrinkToFit();
		bool IsOnHeap() const { return m_buffer.Get() != m_stack.Get(); }

		bool StartsWith( const char* other ) const;
		bool StartsWith( const String& other ) const;

		BASIC_TYPE( String )

		DEFINE_ITERATORS( const char, m_buffer, m_length )

	protected:

		Buffer<char> m_buffer;
		uint m_length { 0 };
		Buffer<char> m_stack;
		
		void Resize( uint length );
		void SetString( const char* data, uint length );
		void Concatenate( const char* other, uint length );
		bool Equals( const char* other, uint length, bool caseless ) const;

		void NullTerminate();

		static const float GrowthFactor;
	};

	//
	// An in-place implementation of string which always allocates a fixed size buffer. 
	//
	template <int Size = 32>
	class InplaceString
		: public String
	{
	public:

		InplaceString()
			: String( m_stackData, Size )
		{
			m_stackData[ 0 ] = '\0';
		}

		explicit InplaceString( const char* other )
			: String( m_stackData, Size )
		{
			SetString( other, (uint) strlen( other ) );
		}

		InplaceString( const char* other, uint length )
			: String( m_stackData, Size )
		{
			SetString( other, length );
		}

		InplaceString( const String& other )
			: String( m_stackData, Size )
		{
			SetString( other.c_str(), other.Length() );
		}

		InplaceString( const InplaceString<Size>& other )
			: String( m_stackData, Size )
		{
			SetString( other.c_str(), other.Length() );
		}

		InplaceString( InplaceString<Size>&& other )
			: String( m_stackData, Size )
		{
			if( other.m_buffer == other.m_stackData )
			{
				SetString( other.c_str(), other.Length() );
			}
			else
			{
				m_buffer.Release();

				m_buffer = other.m_buffer;
				other.m_buffer.Release();

				m_length = other.m_length;
			}
		}

		template <int OtherSize>
		InplaceString( const InplaceString<OtherSize>& other )
			: String( m_stackData, Size )
		{
			SetString( other.c_str(), other.Length() );
		}

		InplaceString<Size>& operator=( const InplaceString<Size>& other )
		{
			SetString( other.c_str(), other.Length() );
			return *this;
		}

		InplaceString<Size>& operator=( const char* other )
		{
			SetString( other, (uint) strlen( other ) );
			return *this;
		}

		virtual ~InplaceString()
		{

		}

		InplaceString<Size> Substring( uint start, uint count = -1 ) const;
		
	private:

		char m_stackData[ Size ];
	};

	typedef InplaceString<8> String8;
	typedef InplaceString<16> String16;
	typedef InplaceString<32> String32;
	typedef InplaceString<64> String64;
	typedef InplaceString<128> String128;
	typedef InplaceString<256> String256;

	template <int Size>
	InplaceString<Size> InplaceString<Size>::Substring( uint start, uint count ) const
	{
		DD_ASSERT( start <= m_length );
		DD_ASSERT( count >= 0 );

		count = std::min( m_length - start, count );

		InplaceString<Size> substring;
		substring.SetString( m_buffer + start, count );

		return substring;
	}
}
