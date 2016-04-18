//
// Frustum.h - A view frustum to be used for entity culling.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PrecompiledHeader.h"
#include "Frustum.h"

#include "AABB.h"
#include "Camera.h"

#include "glm/gtx/transform.hpp"

#include <cmath>

namespace dd
{
	Frustum::Frustum( Camera& camera )
	{
		/*const float vfov = camera.GetVerticalFOV();
		const float si = std::sinf( vfov );
		const float co = std::cosf( vfov );
		const float aspect = camera.GetAspectRatio();

		m_planes[0] = Plane( 0.0f,	-co,	si,				0.0f );
		m_planes[1] = Plane( 0.0f,	co,		si,				0.0f );
		m_planes[2] = Plane( co,	0.0f,	si * aspect,	0.0f );
		m_planes[3] = Plane( -co,	0.0f,	si * aspect,	0.0f );
		m_planes[4] = Plane( 0.0f,	0.0f,	1.0f,			camera.GetFar() );
		m_planes[5] = Plane( 0.0f,	0.0f,	-1.0f,			-camera.GetNear() );*/

		glm::mat4 mvp = camera.GetCameraMatrix() * camera.GetProjection();

		m_planes[0] = Plane(  mvp[0] + mvp[3] );
		m_planes[1] = Plane( -mvp[0] + mvp[3] );
		m_planes[2] = Plane(  mvp[1] + mvp[3] );
		m_planes[3] = Plane( -mvp[1] + mvp[3] );
		m_planes[4] = Plane(  mvp[2] + mvp[3] );
		m_planes[5] = Plane( -mvp[2] + mvp[3] );
	}

	bool Frustum::Intersects( const AABB& bounds ) const
	{
		glm::vec3 corners[8];
		bounds.GetCorners( corners );

		int totalIn = 0;

		for( const Plane& plane : m_planes )
		{
			int inCount = 8;
			bool inside = true;

			glm::vec4 pvertex = glm::vec4(
				plane.Normal().x > 0 ? bounds.Max.x : bounds.Min.x,
				plane.Normal().y > 0 ? bounds.Max.y : bounds.Min.y,
				plane.Normal().z > 0 ? bounds.Max.z : bounds.Min.z,
				1.0f
				);

			pvertex = pvertex * m_invTransform;

			if( plane.DistanceTo( pvertex.xyz() ) < 0 )
				return false;
		}

		// we must be partly in then otherwise
		return true;
	}
}
