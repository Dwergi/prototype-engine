//
// Frustum.h - A view frustum to be used for entity culling.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PCH.h"
#include "Frustum.h"

#include "ICamera.h"

#include "ddm/AABB.h"
#include "ddm/Sphere.h"

#include "ddr/OpenGL.h"
#include "ddr/RenderData.h"
#include "ddr/Shader.h"
#include "ddr/Uniforms.h"
#include "ddr/VAO.h"

namespace ddr
{
	static dd::Service<ddr::ShaderManager> s_shaderManager;

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

	static dd::ConstBuffer<GLushort> s_indexBuffer(s_indices, dd::ArrayLength(s_indices));

	const glm::vec3 s_colours[] =
	{
		glm::vec3(1, 0, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 0, 1),
		glm::vec3(1, 1, 0),
		glm::vec3(1, 0, 1),
		glm::vec3(0, 1, 1)
	};

	Frustum::Frustum()
	{
		m_corners.Set(new glm::vec3[8], 8);
	}

	Frustum::~Frustum()
	{
		glm::vec3* corners = m_corners.Release();
		delete[] corners;
	}

	void Frustum::Update(const ddr::ICamera& camera)
	{
		UpdateFrustum(camera);

		glm::mat4 camera_transform = camera.GetViewMatrix();
		if (m_dirty || m_transform != camera_transform)
		{
			m_transform = camera_transform;
		}
	}

	bool Frustum::Intersects(const ddm::Sphere& bounds) const
	{
		for (const ddm::Plane& plane : m_planes)
		{
			float distance = plane.DistanceTo(bounds.Centre);
			if (distance < -bounds.Radius)
			{
				return false;
			}

			if (abs(distance) < bounds.Radius)
			{
				return true;
			}
		}

		return true;
	}

	bool Frustum::Intersects(const ddm::AABB& bounds) const
	{
		glm::vec3 corners[8];
		bounds.GetCorners(corners);

		for (const ddm::Plane& plane : m_planes)
		{
			bool inside = false;

			for (const glm::vec3& corner : corners)
			{
				if (plane.DistanceTo(corner) > 0)
				{
					inside = true;
					break;
				}
			}

			if (!inside)
			{
				return false;
			}
		}

		// we must be partly in then otherwise
		return true;
	}

	void Frustum::CreateRenderData()
	{
		m_shader = s_shaderManager->Load("mesh");
		ScopedShader shader = m_shader.Access()->UseScoped();

		m_vao.Create("frustum");

		m_vboVertex.Create("frustum");
		m_vao.BindVBO(m_vboVertex, 0, sizeof(m_corners[0]));
		shader->BindPositions(m_vao, m_vboVertex);

		m_renderState.BackfaceCulling = false;
		m_renderState.Blending = true;
		m_renderState.Depth = true;
		m_renderState.DepthWrite = false;

		m_dirty = true;
	}

	void Frustum::Render(const ddr::RenderData& data)
	{
		DD_ASSERT(m_vao.IsValid());

		if (m_dirty)
		{
			m_vboVertex.SetData(m_corners);

			m_dirty = false;
		}

		UniformStorage& uniforms = data.Uniforms();

		ScopedShader shader = m_shader.Access()->UseScoped();
		ScopedRenderState state = m_renderState.UseScoped();

		m_vao.Bind();

		uniforms.Set("Model", m_transform);

		for (int i = 0; i < 6; ++i)
		{
			uniforms.Set("ObjectColour", glm::vec4(s_colours[i], 0.5f));

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void*)(6 * sizeof(GLushort) * i));
		}

		m_vao.Unbind();
	}

	void Frustum::UpdateFrustum(const ddr::ICamera& camera)
	{
		m_vfov = camera.GetVerticalFOV();
		m_aspectRatio = camera.GetAspectRatio();
		m_near = camera.GetNear();
		m_far = camera.GetFar();

		// Work out corners of the frustum
		float tan_fov = std::tanf(m_vfov);

		float near_height = tan_fov * m_near;
		float near_width = near_height * m_aspectRatio;

		float far_height = tan_fov * m_far;
		float far_width = far_height * m_aspectRatio;

		glm::vec3 pos = camera.GetPosition();
		glm::vec3 dir = camera.GetDirection();

		glm::vec3 near_centre = pos + dir * m_near;
		glm::vec3 far_centre = pos + dir * m_far;

		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right = glm::normalize(glm::cross(dir, up));

		up = glm::normalize(glm::cross(right, dir));

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

		m_planes[Planes::Top]	 = ddm::Plane(m_corners[NEAR_TR], m_corners[NEAR_TL], m_corners[FAR_TL]);
		m_planes[Planes::Bottom] = ddm::Plane(m_corners[NEAR_BL], m_corners[NEAR_BR], m_corners[FAR_BR]);
		m_planes[Planes::Left]	 = ddm::Plane(m_corners[NEAR_TL], m_corners[NEAR_BL], m_corners[FAR_BL]);
		m_planes[Planes::Right]  = ddm::Plane(m_corners[NEAR_BR], m_corners[NEAR_TR], m_corners[FAR_BR]);
		m_planes[Planes::Near]	 = ddm::Plane(m_corners[NEAR_TL], m_corners[NEAR_TR], m_corners[NEAR_BR]);
		m_planes[Planes::Far]	 = ddm::Plane(m_corners[FAR_TR], m_corners[FAR_TL], m_corners[FAR_BL]);

		m_dirty = true;
	}

	void* Frustum::operator new(size_t i)
	{
		return _aligned_malloc(i, 16);
	}

	void Frustum::operator delete(void* ptr)
	{
		_aligned_free(ptr);
	}
}
