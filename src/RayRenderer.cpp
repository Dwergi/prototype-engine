//
// RayRenderer.cpp - Renderer for RayComponent.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#include "PCH.h"
#include "RayRenderer.h"

#include "ColourComponent.h"
#include "RayComponent.h"

#include "ddr/Shader.h"

namespace ddr
{
	static dd::Service<ddr::ShaderManager> s_shaderManager;

	static const glm::vec3 s_lines[] =
	{
		// direction
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 1),

		// arrowhead
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0.05, 0.95),
		glm::vec3(0, 0, 1),
		glm::vec3(0, -0.05, 0.95),
		glm::vec3(0, 0, 1),
		glm::vec3(0.05, 0, 0.95),
		glm::vec3(0, 0, 1),
		glm::vec3(-0.05, 0, 0.95)
	};

	static const dd::ConstBuffer<glm::vec3> s_linesBuffer(s_lines, dd::ArrayLength(s_lines));

	RayRenderer::RayRenderer() :
		IRenderer("Ray")
	{
		RequireTag(ddc::Tag::Visible);
		Require<dd::RayComponent>();
		Optional<dd::ColourComponent>();
	}

	RayRenderer::~RayRenderer()
	{

	}

	void RayRenderer::Initialize()
	{
		m_shader = s_shaderManager->Load("line");

		Shader* shader = m_shader.Access();

		m_vao.Create();
		m_vbo.Create(s_linesBuffer);
		m_vao.BindVBO(m_vbo, 0, sizeof(s_linesBuffer[0]));

		shader->BindPositions(m_vao, m_vbo);
	}

	void RayRenderer::Render(const ddr::RenderData& data)
	{
		ScopedShader shader = m_shader.Access()->UseScoped();

		glm::mat4 view_projection = data.Camera().GetProjectionMatrix() * data.Camera().GetViewMatrix();

		auto rays = data.Get<dd::RayComponent>();
		auto colours = data.Get<dd::ColourComponent>();

		for (size_t i = 0; i < data.Size(); ++i)
		{
			const dd::RayComponent& ray = rays[i];

			glm::vec4 clr(1);

			const dd::ColourComponent* colour = colours.Get(i);
			if (colour != nullptr)
			{
				clr = colour->Colour;
			}

			shader->SetUniform("Colour", clr);

			float scale = ray.Ray.HasLength() ? ray.Ray.Length : 100;
			glm::mat4 model = ddm::TransformFromRay(ray.Ray) * glm::scale(glm::vec3(scale));

			shader->SetUniform("ModelViewProjection", view_projection * model);

			OpenGL::DrawArrays(OpenGL::Primitive::Lines, s_linesBuffer.Size());
		}
	}
}
