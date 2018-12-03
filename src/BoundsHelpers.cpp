//
// BoundsHelpers.cpp
// Copyright (C) Sebastian Nordgren 
// December 12th 2018
//

#include "PCH.h"
#include "BoundsHelpers.h"

#include "AABB.h"
#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "Sphere.h"
#include "TransformComponent.h"

namespace dd
{
	bool GetWorldBoundBoxAndSphere( const dd::BoundBoxComponent* bound_box, const dd::BoundSphereComponent* bound_sphere, 
		const dd::TransformComponent& transform, ddm::AABB& aabb, ddm::Sphere& sphere )
	{
		if( GetBoundBoxAndSphere( bound_box, bound_sphere, aabb, sphere ) )
		{
			glm::mat4 m = transform.Transform();
			aabb = aabb.GetTransformed( m );
			sphere = sphere.GetTransformed( m );
			return true;
		}

		return false;
	}

	bool GetBoundBoxAndSphere( const dd::BoundBoxComponent* bound_box, const dd::BoundSphereComponent* bound_sphere,
		ddm::AABB& aabb, ddm::Sphere& sphere )
	{
		aabb = ddm::AABB();
		sphere = ddm::Sphere();

		if( bound_box != nullptr )
		{
			aabb = bound_box->BoundBox;
		}

		if( bound_sphere != nullptr )
		{
			sphere = bound_sphere->Sphere;
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
			sphere = ddm::Sphere( aabb );
		}
		else if( !aabb.IsValid() && sphere.IsValid() )
		{
			aabb = ddm::AABB( sphere );
		}

		return true;
	}
}