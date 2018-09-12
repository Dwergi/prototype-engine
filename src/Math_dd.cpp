#include "PrecompiledHeader.h"
#include "Math_dd.h"

namespace dd
{
	glm::vec3 directionFromPitchYaw( float pitch, float yaw )
	{
		return glm::vec3( glm::cos( pitch ) * glm::sin( yaw ),
			glm::sin( pitch ),
			glm::cos( pitch ) * glm::cos( yaw ) );
	}
}