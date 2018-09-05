#include "PrecompiledHeader.h"
#include "HitTest.h"

#include "Mesh.h"
#include "Ray.h"

namespace dd
{
	bool HitTestTriangle( glm::vec3 ro, glm::vec3 rd, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2 )
	{
		glm::vec3 v1v0 = v1 - v0;
		glm::vec3 v2v0 = v2 - v0;
		glm::vec3 rov0 = ro - v0;

		glm::vec3 n = glm::cross( v1v0, v2v0 );
		glm::vec3 q = glm::cross( rov0, rd );
		float d = 1.0f / glm::dot( n, rd );
		float u = d * glm::dot( -q, v2v0 );
		float v = d * glm::dot( q, v1v0 );
		float t = d * glm::dot( -n, rov0 );

		if( u < 0.0 || u > 1.0 || v < 0.0 || (u + v) > 1.0 )
			return false;
		
		return true;
	}

	bool HitTestMesh( const dd::Ray& ray, const glm::mat4& mesh_transform, const ddr::Mesh& mesh, float& out_distance )
	{
		const dd::ConstBuffer<glm::vec3>& positions = mesh.GetPositions();
		const dd::ConstBuffer<uint>& indices = mesh.GetIndices();

		if( !positions.IsValid() )
		{
			return false;
		}

		AABB transform_bb = mesh.GetBoundBox().GetTransformed( mesh_transform );
		if( !transform_bb.IntersectsRay( ray, out_distance ) )
		{
			return false;
		}

		// transform to mesh space
		glm::vec3 origin = ray.Origin() - mesh_transform[3].xyz;
		glm::mat3 rot = glm::mat3( mesh_transform[0].xyz, mesh_transform[1].xyz, mesh_transform[2].xyz );
		glm::mat3 inv_rot = glm::inverse( rot );

		glm::vec3 dir = ray.Direction() * inv_rot;

		if( indices.IsValid() )
		{
			for( int i = 0; i < indices.Size(); i += 3 )
			{
				uint i0 = indices[i];
				uint i1 = indices[i + 1];
				uint i2 = indices[i + 2];

				glm::vec3 hit_pos;
				if( HitTestTriangle( origin, dir, positions[i0], positions[i1], positions[i2] ) )
				{
					return true;
				}
			}
		}
		else
		{
			for( int i = 0; i < positions.Size(); i += 3 )
			{
				glm::vec3 hit_pos;
				if( HitTestTriangle( origin, dir, positions[i], positions[i + 1], positions[i + 2] ) )
				{
					return true;
				}
			}
		}

		return false;
	}
}
