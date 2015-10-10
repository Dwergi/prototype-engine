//
// Stream.h - A basic wrapper around either a string or a buffer that can be read from or written to.
// Copyright (C) Sebastian Nordgren 
// September 16th 2015
//

#pragma once

namespace dd
{
	class Stream
	{
	public:

		Stream( uint capacity );

		uint Remaining() const;
		void Advance( uint bytes );

	protected:

		uint m_capacity;
		uint m_current;
	};

	class ReadStream
		: public Stream
	{
	public:

		ReadStream( const String& in );
		ReadStream( const void* in, uint capacity );
		ReadStream( const ReadStream& other );

		char PeekByte();
		char ReadByte();
		void Read( String& dst );
		void Read( void* dst, uint bytes );
		void ReadFormat( const char* format, ... );

	private:
		const String* m_strSource;
		const void* m_pSource;
	};

	class WriteStream
		: public Stream
	{
	public:

		WriteStream( String& out );
		WriteStream( void* out, uint capacity );
		WriteStream( const WriteStream& other );

		void Write( const String& str );
		void Write( const void* src, uint bytes );
		void WriteFormat( const char* format, ... );

	private:

		String* m_strDest;
		void* m_pDest;
	};
}