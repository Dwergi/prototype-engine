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

		Stream( int capacity );

		int Remaining() const;
		void Advance( int bytes );
		int Offset() const { return m_current; }

	protected:

		int m_capacity { 0 };
		int m_current { 0 };
	};
	//===================================================================================

	class ReadStream
		: public Stream
	{
	public:

		ReadStream( const void* in, int capacity );
		ReadStream( const ReadStream& other );

		char PeekByte();
		char ReadByte();
		void Read( String& dst );
		void Read( void* dst, int bytes );

		template <typename T>
		T ReadPOD()
		{
			DD_ASSERT( Remaining() > sizeof( T ) );
			DD_ASSERT( m_pSource != nullptr, "You can't read POD using this method to string streams!" );

			const void* src = PointerAdd( m_pSource, m_current );
			Advance( sizeof( T ) );

			return *(const T*) src;
		}

		const void* Data() const { return m_pSource; }
		
	private:
		const void* m_pSource { nullptr };
	};
	//===================================================================================

	class WriteStream
		: public Stream
	{
	public:

		// TODO: Write a separate string stream...
		WriteStream( String& out );
		WriteStream( void* out, int capacity );
		WriteStream( const WriteStream& other );

		void Reset();

		void WriteByte( byte c );
		void Write( const String& str );
		void Write( const char* str );
		void Write( const void* src, int bytes );
		void WriteFormat( const char* format, ... );

		template <typename T>
		void WritePOD( const T& val )
		{
			DD_ASSERT( Remaining() > sizeof( T ) );
			DD_ASSERT( m_pDest != nullptr, "You can't write POD using this method to string streams!" );

			void* dest = PointerAdd( m_pDest, m_current );
			*(T*) dest = val;
			Advance( sizeof( T ) );
		}

	private:

		String* m_strDest { nullptr };
		void* m_pDest { nullptr };
	};
	//===================================================================================
}