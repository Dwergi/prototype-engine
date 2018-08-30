//
// Frustum.h - A view frustum to be used for entity culling.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "Plane.h"
#include "ShaderProgram.h"
#include "VAO.h"
#include "VBO.h"

namespace dd
{
	struct AABB;
}

namespace ddr 
{
	class ICamera;
	struct RenderData;
	struct ShaderProgram;
	struct UniformStorage;

	struct alignas(16) Frustum
	{
	public:

		Frustum();
		~Frustum();

		void CreateRenderData();

		//
		// Check if the frustum intersects with the given bounds.
		//
		bool Intersects( const dd::AABB& bounds ) const;

		void Render( const ddr::RenderData& render_data );
		void Update( const ddr::ICamera& camera );

		void* operator new( size_t i );
		void operator delete( void* ptr);

	private:

		// corners of the frustum in world space
		dd::Buffer<glm::vec3> m_corners;
		// planes of the frustum in world space
		dd::Plane m_planes[6];

		glm::mat4 m_transform;

		VAO m_vao;
		VBO m_vboIndex;
		VBO m_vboVertex;

		bool m_dirty { true };

		float m_vfov { 0.0f };
		float m_aspectRatio { 0.0f };
		float m_near { 0.0f };
		float m_far { 0.0f };

		ShaderHandle m_shader;

		void UpdateFrustum( const ddr::ICamera& camera );
	};
}
