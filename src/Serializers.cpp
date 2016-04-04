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
	__declspec(thread) static char s_temp[ TEMP_BUFFER_SIZE ];

	void* PointerAdd( void* base, uint64 offset )
	{
		return (void*) ((size_t) base + offset);
	}

	namespace Serialize
	{
		base64::encoder s_encoder;
		base64::decoder s_decoder;

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
					s_encoder.encode( str.c_str(), str.Length(), s_temp );
				}
				else
				{
					strcpy_s( s_temp, str.c_str() );
				}

				dst.WriteFormat( "\"%s\"", s_temp );
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
				char c = src.PeekByte();
				DD_ASSERT( c == '"' );
				src.Advance( 1 );

				int iCount = 0;
				
				while( (c = src.ReadByte()) != '"' )
				{
					s_temp[iCount] = c;
					++iCount;
				}

				DD_ASSERT( iCount + 1 < TEMP_BUFFER_SIZE );
				s_temp[iCount] = '\0';

				str = s_temp;

				if( mode == Serialize::Mode::BASE64 )
				{
					s_decoder.decode( str.c_str(), TEMP_BUFFER_SIZE, s_temp );
					str = s_temp;
				}
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
		void SerializePOD<bool>( Mode mode, WriteStream& dst, Variable src )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				dst.Write( src.Data(), sizeof( bool ) );
			}
			else
			{
				String8 value( src.GetValue<bool>() ? "true" : "false" );
				dst.Write( value );
			}
		}

		template<>
		void SerializePOD<char*>( Mode mode, WriteStream& dst, Variable src )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				dst.Write( src.Data(), (uint) strlen( src.GetValue<char*>() ) );
			}
			else
			{
				dst.WriteFormat( "%s", src.GetValue<char*>() );
			}
		}

		template <typename T>
		void DeserializePODInternal( Mode mode, ReadStream& src, Variable dst, const char* format )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				src.Read( &dst.GetValue<T>(), sizeof( T ) );
			}
			else
			{
				src.ReadFormat( format, &dst.GetValue<T>() );
			}
		}

		template<>
		void DeserializePOD<int>( Mode mode, ReadStream& src, Variable dst )
		{
			DeserializePODInternal<int>( mode, src, dst, "%d" );
		}

		template<>
		void DeserializePOD<char>( Mode mode, ReadStream& src, Variable dst )
		{
			DeserializePODInternal<char>( mode, src, dst, "%c" );
		}

		template<>
		void DeserializePOD<int16>( Mode mode, ReadStream& src, Variable dst )
		{
			DeserializePODInternal<int16>( mode, src, dst, "%hd" );
		}

		template<>
		void DeserializePOD<int64>( Mode mode, ReadStream& src, Variable dst )
		{
			DeserializePODInternal<int64>( mode, src, dst, "%lld" );
		}

		template<>
		void DeserializePOD<uint>( Mode mode, ReadStream& src, Variable dst )
		{
			DeserializePODInternal<uint>( mode, src, dst, "%u" );
		}

		template<>
		void DeserializePOD<byte>( Mode mode, ReadStream& src, Variable dst )
		{
			DeserializePODInternal<byte>( mode, src, dst, "%hhu" );
		}

		template<>
		void DeserializePOD<uint16>( Mode mode, ReadStream& src, Variable dst )
		{
			DeserializePODInternal<uint16>( mode, src, dst, "%hu" );
		}

		template<>
		void DeserializePOD<uint64>( Mode mode, ReadStream& src, Variable dst )
		{
			DeserializePODInternal<uint64>( mode, src, dst, "%llu" );
		}

		template<>
		void DeserializePOD<float>( Mode mode, ReadStream& src, Variable dst )
		{
			DeserializePODInternal<float>( mode, src, dst, "%f" );
		}

		template<>
		void DeserializePOD<double>( Mode mode, ReadStream& src, Variable dst )
		{
			DeserializePODInternal<double>( mode, src, dst, "%lf" );
		}

		template<>
		void DeserializePOD<bool>( Mode mode, ReadStream& src, Variable dst )
		{
			if( mode == Serialize::Mode::BINARY )
			{
				src.Read( &dst.GetValue<bool>(), sizeof( bool ) );
			}
			else
			{
				char out[5];
				src.Read( out, 4 );
				out[4] = '\0';

				if( strcmp( out, "true" ) )
				{
					dst.GetValue<bool>() = true;
					return;
				}
				
				src.Read( out + 4, 1 );

				if( strcmp( out, "false" ) )
				{
					dst.GetValue<bool>() = false;
				}
			}
		}

		template<>
		void DeserializePOD<char*>( Mode mode, ReadStream& src, Variable dst )
		{
			String64 out;
			DeserializeString( mode, src, Variable( out ) );
		
			char* dest = dst.GetValue<char*>();
			memcpy( dest, out.c_str(), out.Length() );

			dest[out.Length()] = '\0';
		}
	}
}