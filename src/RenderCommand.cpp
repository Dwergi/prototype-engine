//
// RenderCommand.cpp
// Copyright (C) Sebastian Nordgren 
// December 13th 2018
//

#include "PCH.h"
#include "RenderCommand.h"

namespace ddr
{
	uint DistanceToDepth( float distance, bool transparent )
	{
		constexpr int INT_BITS = 18;
		constexpr int FRACT_BITS = 6;

		constexpr uint MASK = 0x00FFFFFF;

		static_assert(INT_BITS + FRACT_BITS == 24);

		int int_part = glm::clamp( (int) distance, 0, 2 << INT_BITS );
		float flt_part = distance - (int) distance;
		int fract_bits = (int) (flt_part / (1.0f / (2 << FRACT_BITS)));

		int combined = (int_part << FRACT_BITS) + fract_bits;

		if( !transparent )
		{
			// sort front to back if not transparent
			combined = ~combined & MASK;
			return combined;
		}

		return combined & MASK;
	}

	RenderCommand::RenderCommand( CommandType type ) :
		Type( type )
	{

	}
}