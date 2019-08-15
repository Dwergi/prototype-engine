//
// HitTest.cpp - Utilities for hit testing things.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#include "PCH.h"
#include "HitTest.h"

#include "AABB.h"
#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "BoundsHelpers.h"
#include "BVHTree.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Ray.h"
#include "Sphere.h"
#include "TransformComponent.h"
#include "Triangulator.h"

#include <glm/gtx/intersect.hpp>

namespace ddm
{
	bool HitTestMesh( const ddm::Ray& ray, const dd::MeshComponent& mesh_cmp, const dd::TransformComponent& transform_cmp,
		const dd::BoundSphereComponent* bsphere_cmp, const dd::BoundBoxComponent* bbox_cmp,
		float& out_distance, glm::vec3& out_normal )
	{
		ddm::AABB aabb;
		ddm::Sphere sphere;
		if( !dd::GetWorldBoundBoxAndSphere( bbox_cmp, bsphere_cmp, transform_cmp, aabb, sphere ) )
		{
			return false;
		}

		const ddr::Mesh* mesh = mesh_cmp.Mesh.Get();
		if( mesh == nullptr )
		{
			return false;
		}

		return ddm::HitTestMesh( ray, transform_cmp.Transform(), sphere, aabb, *mesh, out_distance, out_normal );
	}

	bool HitTestMesh( const ddm::Ray& ray, const glm::mat4& transform, 
		const ddm::Sphere& bound_sphere, const ddm::AABB& bound_box, 
		const ddr::Mesh& mesh, float& out_distance, glm::vec3& out_normal )
	{
		const dd::ConstBuffer<glm::vec3>& positions = mesh.GetPositions();
		if( !positions.IsValid() )
		{
			return false;
		}

		glm::vec3 position, normal;
		if( !bound_sphere.IntersectsRay( ray, position, normal ) || 
			glm::distance2( ray.Origin(), position ) > (ray.Length * ray.Length) )
		{
			return false;
		}

		float distance;
		if( !bound_box.IntersectsRay( ray, distance ) || distance > ray.Length )
		{
			return false;
		}

		const dd::BVHTree* bvh = mesh.GetBVH();
		if( bvh == nullptr )
		{
			return false;
		}

		// transform to mesh space
		glm::mat4 inv_transform = glm::inverse( transform );

		glm::vec3 origin = (inv_transform * glm::vec4( ray.Origin(), 1 )).xyz;
		glm::vec3 dir = (inv_transform * glm::vec4( ray.Direction(), 0 )).xyz;

		dd::ConstTriangulator triangulator( mesh );

		DD_ASSERT( triangulator.Size() > 0 );

		dd::BVHIntersection intersection = 
			bvh->IntersectsRayFn( ddm::Ray( origin, dir ), [&origin, &dir, &triangulator, &transform, &out_normal]( dd::BVHHandle h )
		{
			dd::ConstTriangle tri = triangulator[(int) h];

			float distance = 0;
			glm::vec2 bary;
			if( glm::intersectRayTriangle( origin, dir, tri.p0, tri.p1, tri.p2, bary, distance ) )
			{
				glm::vec3 normal = (transform * glm::vec4( ddm::NormalFromTriangle( tri.p0, tri.p1, tri.p2 ), 0 )).xyz;
				out_normal = glm::normalize( normal );

				return distance;
			}

			return FLT_MAX;
		} );

		if( intersection.IsValid() && intersection.Distance < ray.Length )
		{
			out_distance = intersection.Distance;
			return true;
		}

		return false;
	}

	bool BoxBoxIntersect(glm::vec2 a_min, glm::vec2 a_max, glm::vec2 b_min, glm::vec2 b_max)
	{
		return glm::all(glm::lessThanEqual(a_min, b_max)) && glm::all(glm::greaterThanEqual(a_max, b_min));
	}
}
