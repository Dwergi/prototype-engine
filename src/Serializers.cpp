//
// Serializers.cpp - Serialization!
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#include "PrecompiledHeader.h"
#include "Serializers.h"

#include "base64/include/encode.h"
#include "base64/include/decode.h"

namespace dd
{
	base64::encoder s_encoder;
	base64::decoder s_decoder;

	void ResetSerializer()
	{
		std::swap( s_encoder, base64::encoder() );
		std::swap( s_decoder, base64::decoder() );
	}

	const char* SerializePOD( SerializationMode mode, const void* data, const char* format, uint size )
	{
		__declspec(thread) static char temp[ 256 ];

		if( mode == SerializationMode::STRING || mode == SerializationMode::JSON )
		{
			sprintf_s( temp, format, data );
		}
		else if( mode == SerializationMode::BASE64 )
		{
			const char* bytes = (const char*) data;
			s_encoder.encode( bytes, (int) size, temp );
		}
		else if( mode == SerializationMode::BINARY )
		{
			memcpy( temp, data, size );
			temp[ size ] = '\0';
		}

		return temp;
	}

	String&& SerializeString( SerializationMode mode, const void* data )
	{
		__declspec(thread) static char temp[ 256 ];

		const String* str = (const String*) data;

		if( mode == SerializationMode::BASE64 )
		{
			s_encoder.encode( str->c_str(), str->Length(), temp );

			return std::move( String256( temp ) );
		}
		
		return std::move( String256( *str ) );
	}

	void DeserializeString( SerializationMode mode, const String& src, void* data )
	{
		__declspec(thread) static char temp[ 256 ];

		String* str = (String*) data;

		strncpy_s( temp, src.c_str(), 256 );

		if( mode == SerializationMode::BASE64 )
		{
			*str = temp;

			s_decoder.decode( str->c_str(), 256, temp );
		}

		*str = temp;
	}
}