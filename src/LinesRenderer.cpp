//
// LinesComponent.h - Renderer for lines.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#include "PCH.h"
#include "LinesRenderer.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "BoundsHelpers.h"
#include "ColourComponent.h"
#include "Frustum.h"
#include "ICamera.h"
#include "LinesComponent.h"
#include "TransformComponent.h"

namespace ddr
{
	dd::Service<ddr::ShaderManager> s_shaderManager;

	LinesRenderer::LinesRenderer() :
		IRenderer("Lines")
	{
		Require<dd::LinesComponent>();
		Require<dd::TransformComponent>();

		Optional<dd::ColourComponent>();
		Optional<dd::BoundBoxComponent>();
		Optional<dd::BoundSphereComponent>();

		m_renderState.BackfaceCulling = false;
		m_renderState.Blending = true;
		m_renderState.Depth = true;
	}

	void LinesRenderer::Initialize()
	{
		m_shader = s_shaderManager->Load("line");
		DD_ASSERT(m_shader.IsValid());

		m_vao.Create("line");
		m_vao.Bind();
		
		m_vboPosition.Create("line");
		m_vao.BindVBO(m_vboPosition, 0, sizeof(glm::vec3));

		m_shader->BindPositions(m_vao, m_vboPosition);

		m_vao.Unbind();
	}

	void LinesRenderer::Render(const ddr::RenderData& render_data)
	{
		auto lines = render_data.Get<dd::LinesComponent>();
		auto transforms = render_data.Get<dd::TransformComponent>();
		auto bound_boxes = render_data.Get<dd::BoundBoxComponent>();
		auto bound_spheres = render_data.Get<dd::BoundSphereComponent>();
		auto colours = render_data.Get<dd::ColourComponent>();

		// calculate total size of buffer
		uint64 total_size = 0;
		for (size_t i = 0; i < render_data.Size(); ++i)
		{
			total_size += lines[i].Points.Size();
		}

		if (total_size == 0)
		{
			return;
		}
		
		// copy data
		std::vector<glm::vec3> line_data;
		line_data.resize(total_size);

		int64 offset = 0;
		for (size_t i = 0; i < render_data.Size(); ++i)
		{
			std::memcpy(line_data.data() + offset, lines[i].Points.Data(), lines[i].Points.Size());
		}

		m_vboPosition.SetData(line_data);

		ScopedShader shader = m_shader.Access()->UseScoped();
		ScopedRenderState scoped_state = m_renderState.UseScoped();
		
		m_vao.Bind();

		const ddr::ICamera& camera = render_data.Camera();
		const glm::mat4 view_projection = camera.GetProjectionMatrix() * camera.GetViewMatrix();

		offset = 0;

		// draw
		for (uint64 i = 0; i < render_data.Size(); ++i)
		{
			const uint64 num_points = lines[i].Points.Size();
			if (num_points == 0)
			{
				continue;
			}

			offset += num_points;

			ddm::AABB aabb;
			ddm::Sphere sphere;
			if (!dd::GetWorldBoundBoxAndSphere(bound_boxes.Get(i), bound_spheres.Get(i), transforms[i], aabb, sphere))
			{
				continue;
			}

			const ddr::Frustum& frustum = camera.GetFrustum();
			if (!frustum.Intersects(sphere) && !frustum.Intersects(aabb))
			{
				continue;
			}

			glm::vec4 colour(1);
			const dd::ColourComponent* colour_cmp = colours.Get(i);
			if (colour_cmp != nullptr)
			{
				colour = colour_cmp->Colour;
			}

			shader->SetUniform("Colour", colour);
			shader->SetUniform("ModelViewProjection", view_projection * transforms[i].Transform());

			OpenGL::DrawArrays(ddr::Primitive::Lines, num_points);
		}

		m_vao.Unbind();
	}
}
