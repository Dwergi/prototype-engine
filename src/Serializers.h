//
// Serializers.h - Serialization!
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#pragma once

namespace dd
{
	namespace Serialize
	{
		enum class Mode : uint
		{
			STRING = 0,
			BINARY = 1,
			BASE64 = 2,
			JSON = 4
		};

		void ResetSerializers();

		void SerializeString( Mode mode, String& out, const void* data );
		void DeserializeString( Mode mode, const String& src, void* data );

		namespace JSON
		{
			template<typename T>
			void SerializePOD( String& out, const T& data, const char* format )
			{
				__declspec(thread) static char temp[ 64 ];

				sprintf_s( temp, format, data );

				out += temp;
			}
		}

		namespace Binary
		{
			template<typename T>
			void SerializePOD( void* out, const T& data )
			{
				memcpy( out, &data, sizeof( T ) );
			}
		}
	}
}