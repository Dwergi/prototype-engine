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
		const float vfov = camera.GetVerticalFOV();
		const float si = std::sinf( vfov );
		const float co = std::cosf( vfov );
		const float aspect = camera.GetAspectRatio();

		m_planes[0] = Plane( 0.0f,	-co,	si,				0.0f );
		m_planes[1] = Plane( 0.0f,	co,		si,				0.0f );
		m_planes[2] = Plane( co,	0.0f,	si * aspect,	0.0f );
		m_planes[3] = Plane( -co,	0.0f,	si * aspect,	0.0f );
		m_planes[4] = Plane( 0.0f,	0.0f,	1.0f,			camera.GetFar() );
		m_planes[5] = Plane( 0.0f,	0.0f,	-1.0f,			-camera.GetNear() );

		m_invTransform = glm::inverse( camera.GetTransform() );
	}

	FrustumState Frustum::Intersects( const AABB& bounds ) const
	{
		glm::vec3 corners[8];
		bounds.GetCorners( corners );

		int totalIn = 0;

		for( const Plane& plane : m_planes )
		{
			int inCount = 8;
			bool inside = true;

			for( const glm::vec3& corner : corners ) 
			{
				glm::vec4 transformed( corner, 1.0f );
				transformed = transformed * m_invTransform;

				if( plane.DistanceTo( transformed.xyz() ) < 0 )
				{
					inside = false;
					--inCount;
				}
			}

			// were all the points outside of plane p?
			if( inCount == 0 )
				return FrustumState::Outside;

			totalIn += inside ? 1 : 0;
		}

		// so if totalIn is 6, then all are inside the view
		if( totalIn == 6 )
			return FrustumState::Inside;

		// we must be partly in then otherwise
		return FrustumState::Intersects;
	}
}
