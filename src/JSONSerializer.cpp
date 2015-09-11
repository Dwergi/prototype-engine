//
// JSONSerializer.h - Serialization! To JSON!
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#include "PrecompiledHeader.h"
#include "JSONSerializer.h"

namespace dd
{
	JSONSerializer::JSONSerializer( String& buffer )
		: m_buffer( buffer ),
		m_offset( 0 )
	{
		dd::Serialize::ResetSerializers();
	}

	JSONSerializer::~JSONSerializer()
	{

	}
}