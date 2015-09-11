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
	namespace Serialize
	{
		base64::encoder s_encoder;
		base64::decoder s_decoder;

		void ResetSerializers()
		{
			std::swap( s_encoder, base64::encoder() );
			std::swap( s_decoder, base64::decoder() );
		}

		void SerializeString( Serialize::Mode mode, String& out, const void* data )
		{
			__declspec(thread) static char temp[ 256 ];

			const String* str = (const String*) data;

			if( mode == Serialize::Mode::BASE64 )
			{
				s_encoder.encode( str->c_str(), str->Length(), temp );
			}
			else
			{
				out += *str;
			}
		}

		void DeserializeString( Serialize::Mode mode, const String& src, void* data )
		{
			__declspec(thread) static char temp[ 256 ];

			String* str = (String*) data;

			strncpy_s( temp, src.c_str(), 256 );

			if( mode == Serialize::Mode::BASE64 )
			{
				*str = temp;

				s_decoder.decode( str->c_str(), 256, temp );
			}

			*str = temp;
		}
	}
}