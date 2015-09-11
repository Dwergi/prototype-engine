//
// JSONSerializer.h - Serialization! To JSON!
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#pragma once

#include "Serializers.h"

namespace dd
{
	class JSONSerializer
	{
	public:
		JSONSerializer( String& buffer );
		~JSONSerializer();

		template <typename T>
		void Serialize( const T& obj );

		template<int S>
		void Serialize( const InplaceString<S>& str );

		template <typename T>
		void Deserialize( T& obj );

	private:
		String& m_buffer;
		uint m_offset;
	};

	template <typename T>
	void JSONSerializer::Serialize( const T& obj )
	{
		const TypeInfo* type = GET_TYPE( T );

		ASSERT( type != nullptr );

		if( type->IsPOD() )
		{
			Serialize::JSON::SerializePOD( m_buffer, obj, type->GetFormat() );
		}
		else if( type->HasCustomSerializers() )
		{
			type->SerializeCustom( Serialize::Mode::JSON, m_buffer, &obj );
		}
		else
		{
			// composite object
		}
	}

	template<int Size>
	void JSONSerializer::Serialize( const InplaceString<Size>& str )
	{
		Serialize::SerializeString( Serialize::Mode::JSON, m_buffer, &str );
	}
}

