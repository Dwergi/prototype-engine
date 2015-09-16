//
// Serializers.cpp - Serialization!
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#include "PrecompiledHeader.h"
#include "Serializers.h"

#include "base64/include/encode.h"
#include "base64/include/decode.h"

#define POD_CASE( Type, Format ) \
	if( !strcmp( format, Format ) ) { SerializePODTemplate( out, *(Type*) data, format ); return; }

namespace dd
{
	const int TEMP_BUFFER_SIZE = 2048;
	__declspec(thread) static char temp[ TEMP_BUFFER_SIZE ];

	void* PointerAdd( void* base, uint offset )
	{
		return (void*) ((size_t) base + offset);
	}

	namespace Serialize
	{
		base64::encoder s_encoder;
		base64::decoder s_decoder;

		void ResetSerializers()
		{
			std::swap( s_encoder, base64::encoder() );
			std::swap( s_decoder, base64::decoder() );
		}

		void SerializeString( Mode mode, WriteStream& dst, Variable src )
		{
			const String& str = src.GetValue<String>();

			if( mode == Mode::BINARY )
			{
				dst.Write( str );
			}
			else
			{
				if( mode == Mode::BASE64 )
				{
					s_encoder.encode( str.c_str(), str.Length(), temp );
				}
				else
				{
					strcpy_s( temp, str.c_str() );
				}

				dst.WriteFormat( "\"%s\"", temp );
			}
		}

		void DeserializeString( Mode mode, ReadStream& src, Variable dst )
		{
			String& str = dst.GetValue<String>();

			if( mode == Mode::BINARY )
			{
				src.Read( str );
			}
			else
			{
				src.ReadFormat( "\"%s\"", temp );

				if( mode == Serialize::Mode::BASE64 )
				{
					str = temp;
					s_decoder.decode( str.c_str(), TEMP_BUFFER_SIZE, temp );
				}

				str = temp;
			}
		}

		template<>
		void SerializePOD<int>( Mode mode, WriteStream& dst, Variable src )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				dst.Write( src.Data(), sizeof( int ) );
			}
			else
			{
				dst.WriteFormat( "%d", src.GetValue<int>() );
			}
		}

		template<>
		void SerializePOD<char>( Mode mode, WriteStream& dst, Variable src )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				dst.Write( src.Data(), sizeof( char ) );
			}
			else
			{
				dst.WriteFormat( "%c", src.GetValue<char>() );
			}
		}

		template<>
		void SerializePOD<int16>( Mode mode, WriteStream& dst, Variable src )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				dst.Write( src.Data(), sizeof( int16 ) );
			}
			else
			{
				dst.WriteFormat( "%hd", src.GetValue<int16>() );
			}
		}

		template<>
		void SerializePOD<int64>( Mode mode, WriteStream& dst, Variable src )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				dst.Write( src.Data(), sizeof( int64 ) );
			}
			else
			{
				dst.WriteFormat( "%lld", src.GetValue<int64>() );
			}
		}

		template<>
		void SerializePOD<uint>( Mode mode, WriteStream& dst, Variable src )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				dst.Write( src.Data(), sizeof( uint ) );
			}
			else
			{
				dst.WriteFormat( "%u", src.GetValue<uint>() );
			}
		}

		template<>
		void SerializePOD<byte>( Mode mode, WriteStream& dst, Variable src )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				dst.Write( src.Data(), sizeof( byte ) );
			}
			else
			{
				dst.WriteFormat( "%hhu", src.GetValue<byte>() );
			}
		}

		template<>
		void SerializePOD<uint16>( Mode mode, WriteStream& dst, Variable src )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				dst.Write( src.Data(), sizeof( uint16 ) );
			}
			else
			{
				dst.WriteFormat( "%hu", src.GetValue<uint16>() );
			}
		}

		template<>
		void SerializePOD<uint64>( Mode mode, WriteStream& dst, Variable src )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				dst.Write( src.Data(), sizeof( uint64 ) );
			}
			else
			{
				dst.WriteFormat( "%llu", src.GetValue<uint64>() );
			}
		}

		template<>
		void SerializePOD<float>( Mode mode, WriteStream& dst, Variable src )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				dst.Write( src.Data(), sizeof( float ) );
			}
			else
			{
				dst.WriteFormat( "%f", src.GetValue<float>() );
			}
		}

		template<>
		void SerializePOD<double>( Mode mode, WriteStream& dst, Variable src )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				dst.Write( src.Data(), sizeof( double ) );
			}
			else
			{
				dst.WriteFormat( "%lf", src.GetValue<double>() );
			}
		}

		template<>
		void SerializePOD<char*>( Mode mode, WriteStream& dst, Variable src )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				dst.Write( src.Data(), strlen( src.GetValue<char*>() ) );
			}
			else
			{
				dst.WriteFormat( "%s", src.GetValue<char*>() );
			}
		}

		template<>
		void DeserializePOD<int>( Mode mode, ReadStream& src, Variable dst )
		{

		}

		template<>
		void DeserializePOD<char>( Mode mode, ReadStream& src, Variable dst )
		{

		}

		template<>
		void DeserializePOD<int16>( Mode mode, ReadStream& src, Variable dst )
		{

		}

		template<>
		void DeserializePOD<int64>( Mode mode, ReadStream& src, Variable dst )
		{

		}

		template<>
		void DeserializePOD<uint>( Mode mode, ReadStream& src, Variable dst )
		{

		}

		template<>
		void DeserializePOD<byte>( Mode mode, ReadStream& src, Variable dst )
		{

		}

		template<>
		void DeserializePOD<uint16>( Mode mode, ReadStream& src, Variable dst )
		{

		}

		template<>
		void DeserializePOD<uint64>( Mode mode, ReadStream& src, Variable dst )
		{

		}

		template<>
		void DeserializePOD<float>( Mode mode, ReadStream& src, Variable dst )
		{

		}

		template<>
		void DeserializePOD<double>( Mode mode, ReadStream& src, Variable dst )
		{

		}

		template<>
		void DeserializePOD<char*>( Mode mode, ReadStream& src, Variable dst )
		{

		}
	}
}