//
// String_dd.h - String implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Iteration.h"
#include "Hash.h"

namespace dd
{
	struct TypeInfo;

	class String
	{
	protected:

		String( char* stackBuffer, int stackCapacity );

	public:

		String( const String& other ) = delete;
		virtual ~String();

		String& operator=( const char* other );
		String& operator=( const String& other );

		String& operator+=( const String& other );
		String& operator+=( const char* other );
		String& operator+=( char other );

		void Append( const char* other, int length );

		bool operator==( const char* other ) const;
		bool operator==( const String& other ) const;

		bool operator!=( const char* other ) const;
		bool operator!=( const String& other ) const;

		bool EqualsCaseless( const char* other ) const;
		bool EqualsCaseless( const String& other ) const;

		int Find( const char* other, int offset = 0 ) const;
		int Find( const String& other, int offset = 0 ) const;

		void ReplaceAll( char src, char target );

		void Clear() { m_length = 0; }

		const char& operator[]( int index ) const { DD_ASSERT( index >= 0 && index < m_length ); return m_buffer[ index ]; }
		char& operator[]( int index ) { DD_ASSERT( index >= 0 && index < m_length ); return m_buffer[index]; }
		
		const char* c_str() const;
		char* data();
		
		// Get a wide copy of this string.
		bool w_str( const Buffer<wchar_t>& buffer ) const;

		int Length() const { return m_length; }
		bool IsEmpty() const { return m_length == 0; }
		void ShrinkToFit();
		bool IsOnHeap() const { return m_buffer.Get() != m_stack.Get(); }

		bool StartsWith( const char* other ) const;
		bool StartsWith( const String& other ) const;

		DD_BASIC_TYPE( String )

		DEFINE_ITERATORS( char, m_buffer.Get(), m_length )

	protected:

		Buffer<char> m_buffer;
		int m_length { 0 };
		Buffer<char> m_stack;
		
		void Resize( int length );
		void SetString( const char* data, int length );
		bool Equals( const char* other, int length, bool caseless ) const;

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
			SetString( other, (int) strlen( other ) );
		}

		InplaceString( const char* other, int length )
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
			if( other.m_buffer.Get() == other.m_stackData )
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
			SetString( other, (int) strlen( other ) );
			return *this;
		}

		virtual ~InplaceString()
		{

		}

		InplaceString<Size> Substring( int start, int count = INT_MAX ) const;
		
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
	InplaceString<Size> InplaceString<Size>::Substring( int start, int count ) const
	{
		DD_ASSERT( start <= m_length );
		DD_ASSERT( count >= 0 );

		count = dd::min( m_length - start, count );

		InplaceString<Size> substring;
		substring.SetString( m_buffer.Get() + start, count );

		return substring;
	}

	template<> inline uint64 Hash<String>( const String& value ) { return HashString( value.c_str(), value.Length() ); }
	template<> inline uint64 Hash<String8>( const String8& value ) { return HashString( value.c_str(), value.Length() ); }
	template<> inline uint64 Hash<String16>( const String16& value ) { return HashString( value.c_str(), value.Length() ); }
	template<> inline uint64 Hash<String32>( const String32& value ) { return HashString( value.c_str(), value.Length() ); }
	template<> inline uint64 Hash<String64>( const String64& value ) { return HashString( value.c_str(), value.Length() ); }
	template<> inline uint64 Hash<String128>( const String128& value ) { return HashString( value.c_str(), value.Length() ); }
	template<> inline uint64 Hash<String256>( const String256& value ) { return HashString( value.c_str(), value.Length() ); }
}

namespace std
{
	template <typename T>
	struct string_hash
	{
		std::size_t operator()( const T& str ) const { return dd::HashString( str.c_str(), str.Length() ); }
	};
	
	template<> struct hash<dd::String> : string_hash<dd::String> {};
	template<> struct hash<dd::String8> : string_hash<dd::String8> {};
	template<> struct hash<dd::String16> : string_hash<dd::String16> {};
	template<> struct hash<dd::String32> : string_hash<dd::String32> {};
	template<> struct hash<dd::String64> : string_hash<dd::String64> {};
	template<> struct hash<dd::String128> : string_hash<dd::String128> {};
	template<> struct hash<dd::String256> : string_hash<dd::String256> {};
}
