#include "PCH.h"
#include "Math_dd.h"

#include "Ray.h"

namespace ddm
{
	bool IsNaN( glm::vec3 v )
	{
		return glm::isnan( v.x ) || glm::isnan( v.y ) || glm::isnan( v.z );
	}

	bool IsNaN( glm::vec4 v )
	{
		return glm::isnan( v.x ) || glm::isnan( v.y ) || glm::isnan( v.z ) || glm::isnan( v.w );;
	}

	bool IsInf( glm::vec3 v )
	{
		return glm::isinf( v.x ) || glm::isinf( v.y ) || glm::isinf( v.z );
	}

	glm::mat4 TransformFromOriginDir( const glm::vec3& origin, const glm::vec3& dir )
	{
		glm::vec3 right = glm::normalize( glm::cross( dir, glm::vec3( 0, 1, 0 ) ) );

		// degenerate case of vector pointing up
		if( ddm::IsNaN( right ) )
		{
			right = glm::vec3( 0, 0, 1 );
		}

		glm::vec3 up = glm::cross( dir, right );

		return glm::mat4(
			glm::vec4( right, 0 ),
			glm::vec4( up, 0 ),
			glm::vec4( dir, 0 ),
			glm::vec4( origin, 1 ) );
	}

	glm::mat4 TransformFromRay( const dd::Ray& ray )
	{
		return TransformFromOriginDir( ray.Origin(), ray.Direction() );
	}

	glm::vec3 DirectionFromPitchYaw( float pitch, float yaw )
	{
		return glm::vec3( glm::cos( pitch ) * glm::sin( yaw ),
			glm::sin( pitch ),
			glm::cos( pitch ) * glm::cos( yaw ) );
	}

	void PitchYawFromDirection( const glm::vec3& dir, float& pitch, float& yaw )
	{
		pitch = asin( dir.y );
		yaw = atan2( dir.x, dir.z );
	}

	glm::quat QuatFromPitchYaw( float pitch, float yaw )
	{
		return glm::angleAxis( pitch, glm::vec3( 1, 0, 0 ) ) * glm::angleAxis( yaw, glm::vec3( 0, 1, 0 ) );
	}

	glm::vec3 NormalFromTriangle( glm::vec3 p0, glm::vec3 p1, glm::vec3 p2 )
	{
		glm::vec3 a = p1 - p0;
		glm::vec3 b = p2 - p0;

		glm::vec3 normal = glm::normalize( glm::cross( a, b ) );
		return normal;
	}
}