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
	class ICamera;
	class ShaderProgram;

	class alignas(16) Frustum
	{
	public:

		Frustum();

		//
		// Check if the frustum intersects with the given bounds.
		//
		bool Intersects( const AABB& bounds ) const;

		void Render( const ICamera& camera, ShaderProgram& shader );
		void ResetFrustum( const ICamera& camera );

		void* operator new( size_t i );
		void operator delete( void* ptr);

	private:

		glm::vec3 m_corners[8];
		Plane m_planes[6];
		glm::mat4 m_transform;

		VAO m_vao;
		VBO m_indices;
		VBO m_vertices;

		void SetCorners( const ICamera& camera );
		void UpdateRenderData();
	};
}
