//
// Entity.h - An entity handle.
// Copyright (C) Sebastian Nordgren 
// August 30th 2018
//

#pragma once

namespace ddc
{
	struct Entity
	{
		friend struct World;

		Entity() :
			Handle( ~0 ),
			Flags( 0 )
		{
		}

		bool IsValid() const { return Handle != ~0; }

		bool operator==( Entity other ) const {	return Handle == other.Handle; }

		union
		{
			struct
			{
				uint ID : 22;
				uint Version : 10;
			};

			uint Handle;
		};

		union
		{
			struct
			{
				byte Alive : 1;
				byte Create : 1;
				byte Destroy : 1;
			};

			uint Flags;
		};
	};
}