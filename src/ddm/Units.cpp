#include "PCH.h"
#include "Units.h"

namespace dd
{
	// speed
	metres speed::operator*( seconds s ) const
	{
		return metres( Value * s.Value );
	}

	speed metres::operator/( seconds s ) const
	{
		return speed( Value / s.Value );
	}

	metres operator"" _m( long double d )
	{
		return metres( (float) d );
	}

	seconds operator"" _s( long double d )
	{
		return seconds( (float) d );
	}

	speed operator"" _ms( long double d )
	{
		return speed( (float) d );
	}
}