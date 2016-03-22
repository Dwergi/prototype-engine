//
// String_dd.cpp - String implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "String_dd.h"

namespace dd
{
	const float String::GrowthFactor = 2.0f;

	String::String( char* stackBuffer, uint stackCapacity )
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

	bool String::Equals( const char* other, uint length, bool caseless ) const
	{
		if( m_length != length )
			return false;

		for( uint i = 0; i < m_length; ++i )
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
		return Equals( other.m_buffer, other.m_length, true );
	}

	bool String::EqualsCaseless( const char* other ) const
	{
		size_t length = strlen( other );
		return Equals( other, (uint) length, true );
	}

	bool String::operator==( const char* other ) const
	{
		ASSERT( other != nullptr );

		size_t length = strlen( other );

		return Equals( other, (uint) length, false );
	}

	bool String::operator==( const String& other ) const
	{
		return Equals( other.m_buffer, other.m_length, false );
	}

	String& String::operator=( const String& other )
	{
		SetString( other.m_buffer, other.m_length );

		return *this;
	}

	String& String::operator=( const char* other )
	{
		ASSERT( other != nullptr );

		SetString( other, (uint) strlen( other ) );

		return *this;
	}

	String& String::operator+=( const String& other )
	{
		Concatenate( other.m_buffer, other.m_length );

		return *this;
	}

	String& String::operator+=( const char* other )
	{
		Concatenate( other, (uint) strlen( other ) );

		return *this;
	}

	String& String::Prepend( const String& other )
	{
		Resize( m_length + other.m_length );

		// shift everything currently in the buffer over by the other's length
		memcpy( m_buffer + other.m_length, m_buffer, m_length );
		
		// copy from other string into the start of the buffer
		memcpy( m_buffer, other.m_buffer, other.m_length );

		m_length += other.m_length;
		NullTerminate();

		return *this;
	}

	int String::Find( const char* other, uint offset ) const
	{
		ASSERT( m_buffer != nullptr );
		ASSERT( other != nullptr );

		if( other == nullptr || offset >= m_length )
			return -1;

		const char* ptr = strstr( m_buffer + offset, other );

		if( ptr == nullptr )
			return -1;

		uint location = (uint) (ptr - m_buffer);

		return location;
	}

	int String::Find( const String& other, uint offset ) const
	{
		ASSERT( m_buffer != nullptr );
		ASSERT( other.m_buffer != nullptr );

		if( offset >= m_length )
			return -1;

		const char* ptr = strstr( m_buffer, other.m_buffer );

		if( ptr == nullptr )
			return -1;

		uint location = (uint) (ptr - m_buffer);

		return location;
	}

	void String::ReplaceAll( char src, char target )
	{
		ASSERT( m_buffer != nullptr );

		for( uint i = 0; i < m_length; ++i )
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
			if( m_buffer[(uint) i] != other[i] )
				return false;
		}

		return true;
	}

	bool String::StartsWith( const String& other ) const
	{
		return StartsWith( other.c_str() );
	}

	void String::Concatenate( const char* buffer, uint other_length )
	{
		uint new_length = m_length + other_length;
		Resize( new_length );

		memcpy( &m_buffer[m_length], buffer, other_length );

		m_length = new_length;
		NullTerminate();
	}

	void String::SetString( const char* data, uint length )
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

		memcpy( m_buffer, data, length );
		m_length = length;

		NullTerminate();
	}

	//
	// Resize to contain a string of the given length. May expand to be larger than the given size.
	// Never shrinks a string.
	//
	void String::Resize( uint length )
	{
		// keep growing string until we hit a size that fits
		uint new_capacity = m_buffer.Size();
		while( length >= new_capacity )
		{
			new_capacity = (uint) (new_capacity * GrowthFactor);
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
			memcpy( m_stack.Get(), m_buffer.Get(), m_length );

			void* ptr = m_buffer.Release();
			delete[] ptr;
			m_buffer = m_stack;
		}
		else
		{
			void* old_buffer = m_buffer.Release();

			m_buffer.Set( new char[m_length + 1], m_length + 1 );

			memcpy( m_buffer, old_buffer, m_length );

			delete[] old_buffer;
		}

		NullTerminate();
	}

	void String::NullTerminate()
	{
		m_buffer[m_length] = '\0';
	}
}