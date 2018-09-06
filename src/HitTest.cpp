#include "PrecompiledHeader.h"
#include "HitTest.h"

#include "Mesh.h"
#include "Ray.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/intersect.hpp>

namespace dd
{
	bool HitTestMesh( const dd::Ray& ray, const glm::mat4& mesh_transform, const ddr::Mesh& mesh, float& out_distance )
	{
		const dd::ConstBuffer<glm::vec3>& positions = mesh.GetPositions();
		if( !positions.IsValid() )
		{
			return false;
		}

		glm::mat4 inv_transform = glm::inverse( mesh_transform );

		// transform to mesh space
		glm::vec4 origin = inv_transform * glm::vec4( ray.Origin(), 1 );
		glm::vec4 dir = inv_transform * glm::vec4( ray.Direction(), 0 );

		Ray inv_ray( origin.xyz, dir.xyz );

		if( !mesh.GetBoundBox().IntersectsRay( ray, out_distance ) )
		{
			return false;
		}

		return true;

		/*const dd::ConstBuffer<uint>& indices = mesh.GetIndices();
		if( indices.IsValid() )
		{
			for( int i = 0; i < indices.Size(); i += 3 )
			{
				uint i0 = indices[i];
				uint i1 = indices[i + 1];
				uint i2 = indices[i + 2];

				glm::vec3 p0 = positions[ i0 ];
				glm::vec3 p1 = positions[ i1 ];
				glm::vec3 p2 = positions[ i2 ];

				glm::vec3 bary;
				if( glm::intersectRayTriangle( origin, dir, p0, p1, p2, bary ) )
				{
					glm::vec4 hit_pos = glm::vec4( bary.x * p0 + bary.y * p1 + bary.z * p2, 1 );
					hit_pos = hit_pos * mesh_transform;

					out_distance = glm::distance( ray.Origin(), glm::vec3( hit_pos.xyz ) );
					return true;
				}

				
			}
		}
		else
		{
			for( int i = 0; i < positions.Size(); i += 3 )
			{
				glm::vec3 p0 = positions[ i + 0 ];
				glm::vec3 p1 = positions[ i + 1 ];
				glm::vec3 p2 = positions[ i + 2 ];

				glm::vec3 bary;
				if( glm::intersectRayTriangle( origin, dir, p0, p1, p2, bary ) )
				{
					glm::vec4 hit_pos = glm::vec4( bary.x * p0 + bary.y * p1 + bary.z * p2, 1 );
					hit_pos = hit_pos * mesh_transform;

					out_distance = glm::distance( ray.Origin(), glm::vec3( hit_pos.xyz ) );
					return true;
				}
			}
		}

		return false;*/
	}
}
