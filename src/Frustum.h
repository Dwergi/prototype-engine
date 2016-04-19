//
// Frustum.h - A view frustum to be used for entity culling.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "Plane.h"
#include "VAO.h"
#include "VBO.h"

namespace dd 
{
	struct AABB;
	class Camera;
	class ShaderProgram;

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
		bool Intersects( const AABB& bounds ) const;

		void Render( Camera& camera, ShaderProgram& shader );

	private:

		glm::vec3 m_corners[8];
		Plane m_planes[6];
		glm::mat4 m_invTransform;
		VAO m_vao;
		VBO m_indices;
		VBO m_vertices;

		void SetCorners( Camera& camera );
	};
}
