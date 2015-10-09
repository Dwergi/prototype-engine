//
// JSONSerializer.h - Serialization! To JSON!
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#pragma once

namespace dd
{
	class ScopedJSONObject;

	class SerializerBase
	{
	public:
		virtual bool Serialize( Variable var ) = 0;
	};
	//===================================================================================

	class DeserializerBase
	{
	public:
		virtual bool Deserialize( Variable var ) = 0;
	};
	//===================================================================================

	class JSONSerializer : public SerializerBase
	{
	public:
		JSONSerializer( WriteStream& stream );
		JSONSerializer( String& buffer );
		virtual ~JSONSerializer();

		virtual bool Serialize( Variable var ) override;

		template <typename T>
		bool Serialize( const T& obj );

	private:
		friend class ScopedJSONObject;

		ScopedJSONObject* m_currentObject;
		WriteStream m_stream;
		uint m_indent;

		void Indent();
		void AddKey( const String& key );
		void AddString( const char* key, const String& value, bool last = false );
	};
	//===================================================================================

	class JSONDeserializer : public DeserializerBase
	{
	public:
		JSONDeserializer( ReadStream& stream );
		JSONDeserializer( const String& buffer );
		virtual ~JSONDeserializer();

		virtual bool Deserialize( Variable var ) override;

		template <typename T>
		bool Deserialize( T& obj );

	private:

		ReadStream m_stream;
	};
	//===================================================================================

	class ScopedJSONObject
	{
	public:
		ScopedJSONObject( JSONSerializer& serializer );
		~ScopedJSONObject();
		void Indent() const;

	private:

		JSONSerializer& m_host;
		ScopedJSONObject* m_previous;
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
}
