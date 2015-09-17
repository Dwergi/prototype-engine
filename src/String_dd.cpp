//
// String_dd.cpp - String implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "String_dd.h"

const float GrowthFactor = 2.0f;

namespace dd
{
	String::String( char* stackBuffer, uint stackCapacity )
		: m_stackCapacity( stackCapacity ),
		m_stackBuffer( stackBuffer )
	{
		m_length = 0;
		m_capacity = m_stackCapacity;
		m_buffer = m_stackBuffer;

		m_buffer[ 0 ] = '\0';
	}

	String::~String()
	{
		// delete heap buffer
		if( m_buffer != m_stackBuffer )
		{
			delete[] m_buffer;
		}
	}

	const char* String::c_str() const
	{
		if( m_buffer == nullptr )
			return nullptr;

		// check that we're null-terminated
		ASSERT( m_buffer[ m_length ] == 0 );

		return m_buffer;
	}

	bool String::Equals( const char* other, uint length ) const
	{
		if( m_length != length )
			return false;

		for( uint i = 0; i < m_length; ++i )
		{
			if( other[ i ] == '\0' )
				return false;

			if( m_buffer[ i ] != other[ i ] )
				return false;
		}

		return true;
	}

	bool String::operator==( const char* other ) const
	{
		ASSERT( other != nullptr );

		size_t length = strlen( other );

		return Equals( other, (uint) length );
	}

	bool String::operator==( const String& other ) const
	{
		return Equals( other.m_buffer, other.m_length );
	}

	String& String::operator=( const String& other )
	{
		SetString( other.m_buffer, other.m_length );

		return *this;
	}

	String& String::operator=( const char* other )
	{
		ASSERT( other != nullptr );

		SetString( other, m_capacity - 1 );

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

	bool String::StartsWith( const char* other ) const
	{
		size_t len = strlen( other );

		for( size_t i = 0; i < len; ++i )
		{
			if( m_buffer[ i ] != other[ i ] )
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

		memcpy( &m_buffer[ m_length ], buffer, other_length );

		m_length = new_length;
		m_buffer[ new_length ] = '\0';		
	}

	void String::SetString( const char* data, uint length )
	{
		if( data == nullptr || length == 0 )
		{
			if( m_buffer != m_stackBuffer )
			{
				delete[] m_buffer;
				m_buffer = m_stackBuffer;
			}

			m_length = 0;

			return;
		}

		Resize( length );

		memcpy( m_buffer, data, length );
		m_length = length;

		// null-terminate
		m_buffer[ m_length ] = '\0';
	}

	//
	// Resize to contain a string of the given length. May expand to be larger than the given size.
	// Never shrinks a string.
	//
	void String::Resize( uint length )
	{
		// keep growing string until we hit a size that fits
		uint new_capacity = m_capacity;
		while( length >= new_capacity )
		{
			new_capacity = (uint) (new_capacity * GrowthFactor);
		}

		if( m_capacity == new_capacity )
			return;

		char* old_buffer = m_buffer;

		m_buffer = new char[ new_capacity ];
		m_capacity = new_capacity;

		// copy old data over if required
		if( m_length > 0 )
			SetString( old_buffer, m_length );

		// delete old buffer if not stack-allocated
		if( old_buffer != m_stackBuffer )
		{
			delete[] old_buffer;
		}
	}

	void String::ShrinkToFit()
	{
		if( m_buffer == m_stackBuffer )
			return;

		// check if we could fit in the local array
		if( (m_length + 1) < m_stackCapacity )
		{
			memcpy( m_stackBuffer, m_buffer, m_length );

			delete[] m_buffer;

			m_buffer = m_stackBuffer;
		}
		else
		{
			char* old_buffer = m_buffer;
			m_buffer = new char[ m_length + 1 ];

			memcpy( m_buffer, old_buffer, m_length );

			delete[] old_buffer;
		}

		m_buffer[ m_length ] = '\0';
	}
}