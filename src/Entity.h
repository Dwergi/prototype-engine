//
// Entity.h - An entity handle.
// Copyright (C) Sebastian Nordgren 
// August 30th 2018
//

#pragma once

namespace ddc
{
	static const int MAX_ENTITIES = 32 * 1024;
	static const int MAX_COMPONENTS = 255;
	static const int MAX_TAGS = 32;

	struct Entity
	{
		friend struct World;

		bool operator==( Entity other ) const { return Handle == other.Handle; }

		bool IsValid() const { return Handle != ~0; }

		union
		{
			struct
			{
				uint ID : 22;
				uint Version : 10;
			};

			uint Handle { ~0u };
		};
	};
}