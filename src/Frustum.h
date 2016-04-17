//
// Frustum.h - A view frustum to be used for entity culling.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "Plane.h"

namespace dd 
{
	struct AABB;
	class Camera;

	enum class FrustumState
	{
		Outside,
		Intersects,
		Inside
	};

	class Frustum
	{
	public:

		//
		// Create a view frustum from the given camera's parameters.
		//
		Frustum( Camera& camera );

		//
		// Check if the frustum intersects with the given bounds.
		//
		FrustumState Intersects( const AABB& bounds ) const;

	private:

		Plane m_planes[6];
		glm::mat4 m_invTransform;
	};
}
