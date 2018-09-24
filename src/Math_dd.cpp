#include "PrecompiledHeader.h"
#include "Math_dd.h"

namespace dd
{
	bool IsNaN( glm::vec3 v )
	{
		return glm::isnan( v.x ) || glm::isnan( v.y ) || glm::isnan( v.z );
	}

	bool IsInf( glm::vec3 v )
	{
		return glm::isinf( v.x ) || glm::isinf( v.y ) || glm::isinf( v.z );
	}

	glm::vec3 directionFromPitchYaw( float pitch, float yaw )
	{
		return glm::vec3( glm::cos( pitch ) * glm::sin( yaw ),
			glm::sin( pitch ),
			glm::cos( pitch ) * glm::cos( yaw ) );
	}

	void pitchYawFromDirection( const glm::vec3& dir, float& pitch, float& yaw )
	{
		pitch = asin( dir.y );
		yaw = atan2( dir.x, dir.z );
	}
}