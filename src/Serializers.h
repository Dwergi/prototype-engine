//
// Serializers.h - Serialization!
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#pragma once

namespace dd
{
	enum class SerializationMode : uint
	{
		STRING = 0,
		BINARY = 1,
		BASE64 = 2,
		JSON = 4
	};

	void ResetSerializer();

	const char* SerializePOD( SerializationMode mode, const void* data, const char* format, uint size );

	String&& SerializeString( SerializationMode mode, const void* data );
	void DeserializeString( SerializationMode mode, const String& src, void* data );
}