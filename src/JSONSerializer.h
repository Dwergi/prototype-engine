//
// JSONSerializer.h - Serialization! To JSON!
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#pragma once

#include "Serializers.h"

namespace dd
{
	class ScopedJSONObject;

	class JSONSerializer
	{
	public:
		JSONSerializer( String& buffer );
		~JSONSerializer();

		void Serialize( Variable var );

		template <typename T>
		void Serialize( const T& obj );

		template <typename T>
		void Deserialize( T& obj );

	private:
		friend class ScopedJSONObject;

		ScopedJSONObject* m_currentObject;
		WriteStream m_stream;
		uint m_offset;
		uint m_indent;

		void Indent();
		void AddKey( const String& key );
		void AddString( const char* key, const String& value );
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
