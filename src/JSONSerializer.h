//
// JSONSerializer.h - Serialization! To JSON!
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#pragma once

#include "Serialization.h"

/*
namespace ArduinoJson
{
	class JsonVariant;
}

namespace dd
{
	class JSONSerializer : public SerializerBase
	{
	public:
		JSONSerializer( WriteStream& stream );
		virtual ~JSONSerializer();

		virtual bool Serialize( Variable var ) override;

		template <typename T>
		bool Serialize( const T& obj );

	private:

		WriteStream m_stream;
	};
	//===================================================================================

	class JSONDeserializer : public DeserializerBase
	{
	public:
		JSONDeserializer( ReadStream& stream );
		virtual ~JSONDeserializer();

		virtual bool Deserialize( Variable var ) override;

		template <typename T>
		bool Deserialize( T& obj );

	private:

		ReadStream m_stream;

		bool SetFromVariant( Variable& var, const ArduinoJson::JsonVariant& variant );
		bool SetArrayFromVariant( Variable& var, const ArduinoJson::JsonVariant& variant );
		bool SetObjectFromVariant( Variable& var, const ArduinoJson::JsonVariant& variant );
	};
	//===================================================================================
	
	template <typename T>
	bool JSONSerializer::Serialize( const T& obj )
	{
		return Serialize( Variable( GET_TYPE( T ), const_cast<T*>( &obj ) ) );
	}

	template <typename T>
	bool JSONDeserializer::Deserialize( T& obj )
	{
		return Deserialize( Variable( GET_TYPE( T ), &obj ) );
	}
	//===================================================================================
}*/
