#include "PrecompiledHeader.h"
#include "BoundsHelpers.h"

#include "AABB.h"
#include "BoundBoxComponent.h"
#include "Sphere.h"
#include "BoundSphereComponent.h"

namespace dd
{
	bool GetWorldBoundBoxAndSphere( const dd::BoundBoxComponent* bound_box, const dd::BoundSphereComponent* bound_sphere, const glm::mat4& transform,
		dd::AABB& aabb, dd::Sphere& sphere )
	{
		if( GetBoundBoxAndSphere( bound_box, bound_sphere, aabb, sphere ) )
		{
			aabb = aabb.GetTransformed( transform );
			sphere = sphere.GetTransformed( transform );
			return true;
		}

		return false;
	}

	bool GetBoundBoxAndSphere( const dd::BoundBoxComponent* bound_box, const dd::BoundSphereComponent* bound_sphere,
		dd::AABB& aabb, dd::Sphere& sphere )
	{
		aabb = dd::AABB();
		sphere = dd::Sphere();

		if( bound_box != nullptr )
		{
			aabb = bound_box->BoundBox;
		}

		if( bound_sphere != nullptr )
		{
			sphere = bound_sphere->BoundSphere;
		}

		if( aabb.IsValid() && sphere.IsValid() )
		{
			return true;
		}
		else if( !aabb.IsValid() && !sphere.IsValid() )
		{
			return false;
		}
		else if( aabb.IsValid() && !sphere.IsValid() )
		{
			sphere = dd::Sphere( aabb );
		}
		else if( !aabb.IsValid() && sphere.IsValid() )
		{
			aabb = dd::AABB( sphere );
		}

		return true;
	}
}