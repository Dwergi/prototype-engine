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
			m_buffer += type->SerializePOD( SerializationMode::JSON, &obj );
		}
		else if( type->HasCustomSerializers() )
		{
			type->SerializeCustom( SerializationMode::JSON, &obj );
		}
		else
		{
			// composite object
		}
	}
}

