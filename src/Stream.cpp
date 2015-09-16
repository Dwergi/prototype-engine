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
	__declspec(thread) static char temp[ 2048 ];

	Stream::Stream( uint capacity )
		: m_capacity( capacity ),
		m_current( 0 )
	{

	}

	void Stream::Advance( uint bytes )
	{
		m_current += bytes;
	}

	uint Stream::Remaining() const
	{
		return m_capacity - m_current;
	}

	ReadStream::ReadStream( const String& in )
		: Stream( in.Length() ),
		m_pSource( nullptr ),
		m_strSource( &in )
	{

	}

	ReadStream::ReadStream( const void* in, uint capacity )
		: Stream( capacity ),
		m_pSource( in ),
		m_strSource( nullptr )
	{

	}

	WriteStream::WriteStream( String& out )
		: Stream( -1 ),
		m_pDest( nullptr ),
		m_strDest( &out )
	{

	}

	WriteStream::WriteStream( void* out, uint capacity )
		: Stream( capacity ),
		m_pDest( out ),
		m_strDest( nullptr )
	{

	}

	void ReadStream::Read( String& dst )
	{
		ASSERT( Remaining() > 0 );

		const char* src = m_pSource != nullptr ? (const char*) m_pSource : m_strSource->c_str();
		src += m_current;

		// we're essentially relying on null-termination here
		dst = src; 

		Advance( dst.Length() );
	}

	void ReadStream::Read( void* dst, uint bytes )
	{
		ASSERT( dst != nullptr );
		ASSERT( Remaining() >= bytes );

		const char* src = m_pSource != nullptr ? (const char*) m_pSource : m_strSource->c_str();

		src += m_current;

		memcpy( dst, src, bytes );

		Advance( bytes );
	}

	void ReadStream::ReadFormat( const char* format, ... )
	{
		const char* src = m_pSource != nullptr ? (const char*) m_pSource : m_strSource->c_str();

		va_list args;
		va_start( args, format );
		int read = sscanf_s( src, format, args );
		va_end( args );

		if( read != -1 )
		{
			Advance( read );
		}
	}

	void WriteStream::Write( const String& str )
	{
		ASSERT( Remaining() > 0 );

		if( m_pDest != nullptr )
		{
			void* dest = PointerAdd( m_pDest, m_current );
			memcpy( dest, str.c_str(), str.Length() );

			void* end = PointerAdd( dest, str.Length() + 1 );
			*(char*)end = 0;
		}
		else
		{
			*m_strDest += str;
		}

		Advance( str.Length() + 1 );
	}

	void WriteStream::Write( const void* src, uint bytes )
	{
		ASSERT( Remaining() >= bytes );

		if( m_pDest != nullptr )
		{
			void* dest = PointerAdd( m_pDest, m_current );

			memcpy( dest, src, bytes );
		}
		else
		{
			memcpy( temp, src, bytes );

			*(temp + bytes + 1) = 0;
		}

		Advance( bytes );
	}

#pragma warning( disable : 4996 )

	void WriteStream::WriteFormat( const char* format, ... )
	{
		ASSERT( Remaining() > 0 );

		void* dest = m_pDest != nullptr ? PointerAdd( m_pDest, m_current ) : temp;

		va_list args;
		va_start( args, format );
		int written = vsnprintf( (char*) dest, Remaining(), format, args );
		va_end( args );

		if( m_strDest != nullptr )
		{
			*m_strDest += temp;
		}

		if( written != -1 )
		{
			Advance( written );
		}
	}

#pragma warning( default : 4996 )
}