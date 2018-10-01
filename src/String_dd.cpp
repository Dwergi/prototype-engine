//
// String_dd.cpp - String implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
#include "String_dd.h"

namespace dd
{
	const float String::GrowthFactor = 2.0f;

	String::String( char* stackBuffer, int stackCapacity )
		: m_stack( stackBuffer, stackCapacity )
	{
		m_length = 0;
		m_buffer = m_stack;

		NullTerminate();
	}

	String::~String()
	{
		// delete heap buffer
		if( m_buffer != m_stack )
		{
			char* buf = m_buffer.Release();
			delete[] buf;
		}

		m_buffer.Release();
		m_stack.Release();
	}

	const char* String::c_str() const
	{
		return m_buffer.Get();
	}

	char* String::data()
	{
		return m_buffer.Access();
	}

	bool String::w_str( const Buffer<wchar_t>& buffer ) const
	{
		if( buffer.Size() <= m_length )
			return false;

		size_t count;
		return mbstowcs_s( &count, buffer.Access(), buffer.Size() / 4, m_buffer.Get(), m_length ) > 0;
	}

	bool String::Equals( const char* other, int length, bool caseless ) const
	{
		if( m_length != length )
			return false;

		for( int i = 0; i < m_length; ++i )
		{
			if( other[i] == '\0' )
				return false;

			if( caseless )
			{
				if( tolower( m_buffer[i] ) != tolower( other[i] ) )
					return false;
			}
			else
			{
				if( m_buffer[i] != other[i] )
					return false;
			}
		}

		return true;
	}

	bool String::EqualsCaseless( const String& other ) const
	{
		return Equals( other.m_buffer.Get(), other.m_length, true );
	}

	bool String::EqualsCaseless( const char* other ) const
	{
		size_t length = strlen( other );
		return Equals( other, (int) length, true );
	}

	bool String::operator==( const char* other ) const
	{
		DD_ASSERT( other != nullptr );

		size_t length = strlen( other );

		return Equals( other, (int) length, false );
	}

	bool String::operator==( const String& other ) const
	{
		return Equals( other.m_buffer.Get(), other.m_length, false );
	}

	bool String::operator!=( const char* other ) const
	{
		return !(operator==( other ));
	}

	bool String::operator!=( const String& other ) const
	{
		return !(operator==( other ));
	}

	String& String::operator=( const String& other )
	{
		SetString( other.m_buffer.Get(), other.m_length );

		return *this;
	}

	String& String::operator=( const char* other )
	{
		DD_ASSERT( other != nullptr );

		SetString( other, (int) strlen( other ) );

		return *this;
	}

	String& String::operator+=( const String& other )
	{
		Append( other.m_buffer.Get(), other.m_length );

		return *this;
	}

	String& String::operator+=( const char* other )
	{
		Append( other, (int) strlen( other ) );

		return *this;
	}

	int String::Find( const char* other, int offset ) const
	{
		DD_ASSERT( m_buffer.Get() != nullptr );
		DD_ASSERT( other != nullptr );

		if( other == nullptr || offset >= m_length )
			return -1;

		const char* ptr = strstr( m_buffer.Get() + offset, other );

		if( ptr == nullptr )
			return -1;

		int location = (int) (ptr - m_buffer.Get());

		return location;
	}

	int String::Find( const String& other, int offset ) const
	{
		DD_ASSERT( m_buffer.Get() != nullptr );
		DD_ASSERT( other.m_buffer.Get() != nullptr );

		if( offset >= m_length )
			return -1;

		const char* ptr = strstr( m_buffer.Get(), other.m_buffer.Get() );

		if( ptr == nullptr )
			return -1;

		int location = (int) (ptr - m_buffer.Get());

		return location;
	}

	void String::ReplaceAll( char src, char target )
	{
		DD_ASSERT( m_buffer.Get() != nullptr );

		for( int i = 0; i < m_length; ++i )
		{
			if( m_buffer[i] == src )
				m_buffer[i] = target;
		}
	}

	String& String::operator+=( char other )
	{
		Resize( m_length + 1 );

		m_buffer[m_length] = other;
		++m_length;

		NullTerminate();

		return *this;
	}

	bool String::StartsWith( const char* other ) const
	{
		size_t len = strlen( other );

		for( size_t i = 0; i < len; ++i )
		{
			if( m_buffer[(int) i] != other[i] )
				return false;
		}

		return true;
	}

	bool String::StartsWith( const String& other ) const
	{
		return StartsWith( other.c_str() );
	}

	void String::Append( const char* buffer, int other_length )
	{
		if( other_length == 0 )
			return;

		int new_length = m_length + other_length;
		Resize( new_length );

		memcpy( &m_buffer[m_length], buffer, other_length );

		m_length = new_length;
		NullTerminate();
	}

	void String::SetString( const char* data, int length )
	{
		if( data == nullptr || length == 0 )
		{
			if( m_buffer != m_stack )
			{
				void* ptr = m_buffer.Release();
				delete[] ptr;

				m_buffer = m_stack;
			}

			m_length = 0;

			return;
		}

		Resize( length );

		memcpy( m_buffer.Access(), data, length );
		m_length = length;

		NullTerminate();
	}

	//
	// Resize to contain a string of the given length. May expand to be larger than the given size.
	// Never shrinks a string.
	//
	void String::Resize( int length )
	{
		// keep growing string until we hit a size that fits
		int new_capacity = m_buffer.Size();
		while( length >= new_capacity )
		{
			new_capacity = (int) (new_capacity * GrowthFactor);
		}

		if( m_buffer.Size() == new_capacity )
			return;

		char* old_buffer = m_buffer.Release();

		m_buffer.Set( new char[new_capacity], new_capacity );

		// copy old data over if required
		if( m_length > 0 )
			SetString( old_buffer, m_length );

		// delete old buffer if not stack-allocated
		if( old_buffer != m_stack.Get() )
		{
			delete[] old_buffer;
		}
	}

	void String::ShrinkToFit()
	{
		if( m_buffer == m_stack )
			return;

		// check if we could fit in the local array
		if( (m_length + 1) < m_stack.Size() )
		{
			memcpy( m_stack.Access(), m_buffer.Get(), m_length );

			void* ptr = m_buffer.Release();
			delete[] ptr;
			m_buffer = m_stack;
		}
		else
		{
			void* old_buffer = m_buffer.Release();

			m_buffer.Set( new char[m_length + 1], m_length + 1 );

			memcpy( m_buffer.Access(), old_buffer, m_length );

			delete[] old_buffer;
		}

		NullTerminate();
	}

	void String::NullTerminate()
	{
		m_buffer[m_length] = '\0';
	}
}