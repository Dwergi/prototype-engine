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
		IRenderer( "Lines" )
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
		m_shader = s_shaderManager->Load( "line" );
		DD_ASSERT( m_shader.IsValid() );

		Shader* shader = m_shader.Access();
		DD_ASSERT( shader != nullptr );

		ScopedShader scoped_shader = shader->UseScoped();

		m_vao.Create();
		m_vao.Bind();

		m_vboPosition.Create(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
		m_vboPosition.Bind();
		shader->BindPositions();
		m_vboPosition.Unbind();

		m_vao.Unbind();
	}

	void LinesRenderer::Render(const ddr::RenderData& render_data)
	{
		auto lines = render_data.Get<dd::LinesComponent>();
		auto transforms = render_data.Get<dd::TransformComponent>();
		auto bound_boxes = render_data.Get<dd::BoundBoxComponent>();
		auto bound_spheres = render_data.Get<dd::BoundSphereComponent>();
		auto colours = render_data.Get<dd::ColourComponent>();

		const ddr::ICamera& camera = render_data.Camera();

		Shader* shader = m_shader.Access();
		ScopedShader scoped_shader = shader->UseScoped();
		ScopedRenderState scoped_state = m_renderState.UseScoped();

		glm::mat4 view_projection = camera.GetProjectionMatrix() * camera.GetViewMatrix();

		for (size_t i = 0; i < render_data.Size(); ++i)
		{
			if (lines[i].Points.Size() == 0)
			{
				continue;
			}

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

			m_vao.Bind();

			m_vboPosition.Bind();
			m_vboPosition.SetData(dd::ConstBuffer<glm::vec3>(lines[i].Points.Data(), lines[i].Points.Size()));
			m_vboPosition.CommitData();

			OpenGL::DrawArrays(OpenGL::Primitive::Lines, lines[i].Points.Size());

			m_vboPosition.Unbind();

			m_vao.Unbind();
		}
	}
}
