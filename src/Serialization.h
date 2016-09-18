#pragma once

#include "Stream.h"

namespace dd
{
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
}

