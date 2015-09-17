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

		virtual void Serialize( Variable var ) = 0;
	};

	class DeserializerBase
	{
	public:

		virtual void Deserialize( Variable var ) = 0;
	};

	class JSONSerializer : public SerializerBase
	{
	public:
		JSONSerializer( WriteStream& stream );
		JSONSerializer( String& buffer );
		virtual ~JSONSerializer();

		virtual void Serialize( Variable var );

		template <typename T>
		void Serialize( const T& obj );

	private:
		friend class ScopedJSONObject;

		ScopedJSONObject* m_currentObject;
		WriteStream m_stream;
		uint m_indent;

		void Indent();
		void AddKey( const String& key );
		void AddString( const char* key, const String& value, bool last = false );
	};

	class JSONDeserializer : public DeserializerBase
	{
	public:
		JSONDeserializer( ReadStream& stream );
		JSONDeserializer( const String& buffer );
		virtual ~JSONDeserializer();

		virtual void Deserialize( Variable var );

		template <typename T>
		void Deserialize( T& obj );
	};

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

	template <typename T>
	void JSONSerializer::Serialize( const T& obj )
	{
		Serialize( Variable( GET_TYPE( T ), const_cast<T*>( &obj ) ) );
	}
}
