#include "PrecompiledHeader.h"
#include "HitTest.h"

#include "AABB.h"
#include "Sphere.h"
#include "BVHTree.h"
#include "Mesh.h"
#include "Ray.h"

#include <glm/gtx/intersect.hpp>

namespace dd
{
	bool HitTestMesh( const dd::Ray& ray, const glm::mat4& transform, 
		const dd::Sphere& bound_sphere, const dd::AABB& bound_box, 
		const ddr::Mesh& mesh, float& out_distance )
	{
		const dd::ConstBuffer<glm::vec3>& positions = mesh.GetPositions();
		if( !positions.IsValid() )
		{
			return false;
		}

		if( !bound_sphere.IntersectsRay( ray ) )
		{
			return false;
		}

		if( !bound_box.IntersectsRay( ray ) )
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

		const dd::ConstBuffer<uint>& indices = mesh.GetIndices();
		if( indices.IsValid() )
		{
			BVHIntersection intersection = 
				bvh->IntersectsRayFn( dd::Ray( origin, dir ), [&origin, &dir, &positions, &indices]( size_t tri )
			{
				glm::vec3 p0 = positions[indices[tri * 3 + 0]];
				glm::vec3 p1 = positions[indices[tri * 3 + 1]];
				glm::vec3 p2 = positions[indices[tri * 3 + 2]];

				glm::vec3 bary;
				if( glm::intersectRayTriangle( origin, dir, p0, p1, p2, bary ) )
				{
					return bary.z;
				}

				return FLT_MAX;
			} );

			out_distance = intersection.Distance;
			return true;
		}
		else
		{
			BVHIntersection intersection = 
				bvh->IntersectsRayFn( dd::Ray( origin, dir ), [&origin, &dir, &positions, &indices]( size_t tri )
			{
				glm::vec3 p0 = positions[tri * 3 + 0];
				glm::vec3 p1 = positions[tri * 3 + 1];
				glm::vec3 p2 = positions[tri * 3 + 2];

				glm::vec3 bary;
				if( glm::intersectRayTriangle( origin, dir, p0, p1, p2, bary ) )
				{
					return bary.z;
				}

				return FLT_MAX;
			} );

			out_distance = intersection.Distance;
			return true;
		}

		return false;
	}
}
