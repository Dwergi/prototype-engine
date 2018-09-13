#include "PrecompiledHeader.h"
#include "HitTest.h"

#include "AABB.h"
#include "BoundSphere.h"
#include "Mesh.h"
#include "Ray.h"

#include <glm/gtx/intersect.hpp>

namespace dd
{
	bool HitTestMesh( const dd::Ray& ray, const glm::mat4& transform, 
		const dd::BoundSphere& bound_sphere, const dd::AABB& bound_box, 
		const ddr::Mesh& mesh, float& out_distance )
	{
		const dd::ConstBuffer<glm::vec3>& positions = mesh.GetPositions();

		if( !positions.IsValid() )
		{
			return false;
		}

		if( !bound_sphere.IntersectsRay( ray ) )
		{

		}

		if( !bound_box.IntersectsRay( ray ) )
		{
			return false;
		}

		// transform to mesh space
		glm::mat4 inv_transform = glm::inverse( transform );
		
		glm::vec3 origin = (inv_transform * glm::vec4( ray.Origin(), 1 )).xyz;
		glm::vec3 dir = (inv_transform * glm::vec4( ray.Direction(), 0 )).xyz;
		dir = glm::normalize( dir );

		const dd::ConstBuffer<uint>& indices = mesh.GetIndices();

		if( indices.IsValid() )
		{
			for( int i = 0; i < indices.Size(); i += 3 )
			{
				glm::vec3 p0 = positions[indices[i + 0]];
				glm::vec3 p1 = positions[indices[i + 1]];
				glm::vec3 p2 = positions[indices[i + 2]];

				glm::vec3 bary; 
				if( glm::intersectRayTriangle( origin, dir, p0, p1, p2, bary ) )
				{
					// not actually barycentric for Z, in fact just a distance...
					// Thanks GLM docs!
					out_distance = bary.z;
					return true;
				}
			}
		}
		else
		{
			for( int i = 0; i < positions.Size(); i += 3 )
			{
				glm::vec3 p0 = positions[i + 0];
				glm::vec3 p1 = positions[i + 1];
				glm::vec3 p2 = positions[i + 2];

				glm::vec3 bary;
				if( glm::intersectRayTriangle( origin, dir, p0, p1, p2, bary ) )
				{
					out_distance = bary.z;
					return true;
				}
			}
		}

		return false;
	}
}
