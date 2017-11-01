//
// Stream.cpp - A basic wrapper around either a string or a buffer that can be read from or written to.
// Copyright (C) Sebastian Nordgren 
// September 16th 2015
//

#include "PrecompiledHeader.h"
#include "Stream.h"

#include <stdarg.h>

namespace dd
{
	__declspec(thread) static char s_temp[ 2048 ];

	Stream::Stream( int capacity )
		: m_capacity( capacity )
	{

	}

	void Stream::Advance( int bytes )
	{
		m_current += bytes;
	}

	int Stream::Remaining() const
	{
		if( m_capacity == -1 )
			return INT_MAX;

		return m_capacity - m_current;
	}
	//----------------------------------------------------------------------------

	ReadStream::ReadStream( const void* in, int capacity )
		: Stream( capacity ),
		m_pSource( in )
	{

	}

	ReadStream::ReadStream( const ReadStream& other )
		: Stream( other.m_capacity ),
		m_pSource( other.m_pSource )
	{
		m_current = other.m_current;
	}

	char ReadStream::PeekByte()
	{
		char dst;
		Read( &dst, 1 );
		--m_current;
		return dst;
	}

	char ReadStream::ReadByte()
	{
		char dst;
		Read( &dst, 1 );
		return dst;
	}

	void ReadStream::Read( String& dst )
	{
		DD_ASSERT( Remaining() > 0 );

		const char* src = (const char*) m_pSource;
		src += m_current;

		// we're essentially relying on null-termination here
		dst = src; 

		Advance( dst.Length() + 1 );
	}

	void ReadStream::Read( void* dst, int bytes )
	{
		DD_ASSERT( dst != nullptr );
		DD_ASSERT( Remaining() >= bytes );

		const char* src = (const char*) m_pSource;
		src += m_current;

		memcpy( dst, src, bytes );

		Advance( bytes );
	}
	//----------------------------------------------------------------------------

	WriteStream::WriteStream( String& out )
		: Stream( -1 ),
		m_pDest( nullptr ),
		m_strDest( &out )
	{

	}

	WriteStream::WriteStream( void* out, int capacity )
		: Stream( capacity ),
		m_pDest( out ),
		m_strDest( nullptr )
	{

	}

	WriteStream::WriteStream( const WriteStream& other )
		: Stream( other.m_capacity ),
		m_pDest( other.m_pDest ),
		m_strDest( other.m_strDest )
	{
		m_current = other.m_current;
	}

	void WriteStream::Reset()
	{
		m_current = 0;
	}

	void WriteStream::WriteByte( byte c )
	{
		DD_ASSERT( Remaining() > 0 );

		if( m_pDest != nullptr )
		{
			void* dest = PointerAdd( m_pDest, m_current );
			*(byte*) dest = c;
		}
		else
		{
			*m_strDest += c;
		}

		Advance( 1 );
	}

	void WriteStream::Write( const String& str )
	{
		DD_ASSERT( Remaining() >= str.Length() );

		if( m_pDest != nullptr )
		{
			void* dest = PointerAdd( m_pDest, m_current );
			memcpy( dest, str.c_str(), str.Length() );

			void* end = PointerAdd( dest, str.Length() );
			*(char*) end = 0;
		}
		else
		{
			*m_strDest += str;
		}

		Advance( str.Length() + 1 );
	}

	void WriteStream::Write( const char* str )
	{
		Write( str, (int) strlen( str ) );

		if( m_pDest != nullptr )
		{
			WriteByte( 0 );
		}
	}

	void WriteStream::Write( const void* src, int bytes )
	{
		DD_ASSERT( Remaining() >= bytes );

		if( m_pDest != nullptr )
		{
			void* dest = PointerAdd( m_pDest, m_current );

			memcpy( dest, src, bytes );
		}
		else
		{
			memcpy( s_temp, src, bytes );

			*(s_temp + bytes) = 0;

			*m_strDest += s_temp;
		}

		Advance( bytes );
	}

#pragma warning( disable : 4996 )

	void WriteStream::WriteFormat( const char* format, ... )
	{
		DD_ASSERT( Remaining() > 0 );

		void* dest = m_pDest != nullptr ? PointerAdd( m_pDest, m_current ) : s_temp;

		va_list args;
		va_start( args, format );
		int written = vsnprintf( (char*) dest, Remaining(), format, args );
		va_end( args );

		if( m_strDest != nullptr )
		{
			*m_strDest += s_temp;
		}

		if( written != -1 )
		{
			Advance( written );
		}
	}

#pragma warning( default : 4996 )
}