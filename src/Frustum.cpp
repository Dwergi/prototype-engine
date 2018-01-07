//
// Frustum.h - A view frustum to be used for entity culling.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PrecompiledHeader.h"
#include "Frustum.h"

#include "AABB.h"
#include "ICamera.h"
#include "ShaderProgram.h"
#include "VAO.h"

#include "GL/gl3w.h"

#include "glm/gtx/transform.hpp"

#include <cmath>

namespace dd
{
	enum Corners
	{
		NEAR_TR = 0,
		NEAR_TL,
		NEAR_BL,
		NEAR_BR,
		FAR_TR,
		FAR_TL,
		FAR_BL,
		FAR_BR
	};

	enum Planes
	{
		Top,
		Bottom,
		Left,
		Right,
		Near,
		Far
	};

	const GLushort s_indices[] =
	{
		// near plane
		NEAR_TL, NEAR_BR, NEAR_TR,
		NEAR_TL, NEAR_BL, NEAR_BR,

		// far plane
		FAR_TL, FAR_TR, FAR_BR,
		FAR_TL, FAR_BR, FAR_BL,

		// bottom plane
		NEAR_BL, FAR_BR, NEAR_BR,
		NEAR_BL, FAR_BL, FAR_BR,

		// top plane
		NEAR_TL, NEAR_TR, FAR_TR,
		NEAR_TL, FAR_TR, FAR_TL,

		// left plane
		NEAR_TL, FAR_TL, NEAR_BL,
		NEAR_BL, FAR_TL, FAR_BL,

		// right plane
		NEAR_TR, NEAR_BR, FAR_TR,
		NEAR_BR, FAR_BR, FAR_TR
	};

	static Buffer<GLushort> s_indexBuffer( const_cast<GLushort*>( s_indices ), sizeof( s_indices ) / sizeof( GLushort ) );

	const glm::vec3 s_colours[] =
	{
		glm::vec3( 1, 0, 0 ),
		glm::vec3( 0, 1, 0 ),
		glm::vec3( 0, 0, 1 ),
		glm::vec3( 1, 1, 0 ),
		glm::vec3( 1, 0, 1 ),
		glm::vec3( 0, 1, 1 )
	};

	Frustum::Frustum()
	{
		m_corners.Set( new glm::vec3[8], 8 );
	}

	Frustum::~Frustum()
	{
		glm::vec3* corners = m_corners.Release();
		delete[] corners;
	}

	void Frustum::Update( const ICamera& camera )
	{
		UpdateFrustum( camera );

		glm::mat4 camera_transform = camera.GetCameraMatrix();
		if( m_dirty || m_transform != camera_transform )
		{
			m_transform = camera_transform;
		}
	}

	bool Frustum::Intersects( const AABB& bounds ) const
	{
		glm::vec3 corners[8];
		bounds.GetCorners( corners );

		for( const Plane& plane : m_planes )
		{
			bool inside = false;

			for( const glm::vec3& corner : corners )
			{
				if( plane.DistanceTo( corner ) > 0 )
				{
					inside = true;
					break;
				}
			}

			if( !inside )
			{
				return false;
			}
		}

		// we must be partly in then otherwise
		return true;
	}

	void Frustum::CreateRenderData( ShaderHandle shader_h )
	{
		m_shader = shader_h;

		m_vao.Create();

		m_vao.Bind();

		m_vboVertex.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboVertex.Bind();
		m_vboVertex.SetData( m_corners );

		m_vboIndex.Create( GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboIndex.Bind();
		m_vboIndex.SetData( s_indexBuffer );

		ShaderProgram& shader = *m_shader.Get();
		shader.Use( true );
		shader.BindPositions();

		shader.Use( false );

		m_vao.Unbind();
	}

	void Frustum::Render( const ICamera& camera )
	{
		if( !m_vao.IsValid() )
		{
			CreateRenderData( m_shader );
		}

		m_vao.Bind();

		if( m_dirty )
		{
			m_vboVertex.Update();

			m_dirty = false;
		}

		ShaderProgram& shader = *m_shader.Get();

		shader.Use( true );

		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		shader.SetUniform( "Model", m_transform );
		shader.SetUniform( "View", camera.GetCameraMatrix() );
		shader.SetUniform( "Projection", camera.GetProjectionMatrix() );

		for( int i = 0; i < 6; ++i )
		{
			shader.SetUniform( "ObjectColour", glm::vec4( s_colours[i], 0.5f ) );

			glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void*) (6 * sizeof(GLushort) * i) );
		}

		glDisable( GL_BLEND );

		shader.Use( false );

		m_vao.Unbind();
	}
	
	void Frustum::UpdateFrustum( const ICamera& camera )
	{
		m_vfov = camera.GetVerticalFOV();
		m_aspectRatio = camera.GetAspectRatio();
		m_near = camera.GetNear();
		m_far = camera.GetFar();
		
		// Work out corners of the frustum
		float tan_fov = std::tanf( m_vfov );

		float near_height = tan_fov * m_near;
		float near_width = near_height * m_aspectRatio;

		float far_height = tan_fov * m_far;
		float far_width = far_height * m_aspectRatio;

		glm::vec3 pos = camera.GetPosition();
		glm::vec3 dir = camera.GetDirection();

		glm::vec3 near_centre = pos + dir * m_near;
		glm::vec3 far_centre = pos + dir * m_far;

		glm::vec3 up = glm::vec3( 0, 1, 0 );
		glm::vec3 right = glm::normalize( glm::cross( dir, up ) );

		up = glm::normalize( glm::cross( right, dir ) );

		glm::vec3 near_up = up * near_height;
		glm::vec3 near_right = right * near_width;

		// near corners - top right, top left, bottom left, bottom right
		m_corners[NEAR_TR] = near_centre + near_up + near_right;
		m_corners[NEAR_TL] = near_centre + near_up - near_right;
		m_corners[NEAR_BL] = near_centre - near_up - near_right;
		m_corners[NEAR_BR] = near_centre - near_up + near_right;

		glm::vec3 far_up = up * far_height;
		glm::vec3 far_right = right * far_width;

		// far corners - top right, top left, bottom left, bottom right
		m_corners[FAR_TR] = far_centre + far_up + far_right;
		m_corners[FAR_TL] = far_centre + far_up - far_right;
		m_corners[FAR_BL] = far_centre - far_up - far_right;
		m_corners[FAR_BR] = far_centre - far_up + far_right;

		m_planes[Planes::Top]	 = Plane( m_corners[NEAR_TR], m_corners[NEAR_TL], m_corners[FAR_TL] );
		m_planes[Planes::Bottom] = Plane( m_corners[NEAR_BL], m_corners[NEAR_BR], m_corners[FAR_BR] );
		m_planes[Planes::Left]	 = Plane( m_corners[NEAR_TL], m_corners[NEAR_BL], m_corners[FAR_BL] );
		m_planes[Planes::Right]	 = Plane( m_corners[NEAR_BR], m_corners[NEAR_TR], m_corners[FAR_BR] );
		m_planes[Planes::Near]	 = Plane( m_corners[NEAR_TL], m_corners[NEAR_TR], m_corners[NEAR_BR] );
		m_planes[Planes::Far]	 = Plane( m_corners[FAR_TR], m_corners[FAR_TL], m_corners[FAR_BL] );

		m_dirty = true;
	}

	void* Frustum::operator new( size_t i )
	{
		return _aligned_malloc( i, 16 );
	}

	void Frustum::operator delete( void* ptr )
	{
		_aligned_free( ptr );
	}
}
