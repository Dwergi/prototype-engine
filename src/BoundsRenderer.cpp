//
// BoundsRenderer.cpp - A debug renderer for bounds.
// Copyright (C) Sebastian Nordgren 
// September 13th 2018
//

#include "PCH.h"
#include "BoundsRenderer.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "BoundsHelpers.h"
#include "Input.h"
#include "MeshUtils.h"
#include "TransformComponent.h"

#include "ddr/OpenGL.h"
#include "ddr/ShaderPart.h"
#include "ddr/Shader.h"

namespace ddr
{
	static dd::Service<ddr::ShaderManager> s_shaderManager;
	static dd::Service<dd::Input> s_input;

	static const glm::vec3 s_corners[] =
	{
		// bottom
		glm::vec3(0, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(0, 0, 1),
		glm::vec3(1, 0, 1),

		// top
		glm::vec3(0, 1, 0),
		glm::vec3(1, 1, 0),
		glm::vec3(0, 1, 1),
		glm::vec3(1, 1, 1)
	};

	static dd::ConstBuffer<glm::vec3> s_cornersBuffer(s_corners, std::size(s_corners));

	static const uint s_indices[] =
	{
		// bottom
		0,1,	1,3,	3,2,	2,0,
		// top
		4,5,	5,7,	7,6,	6,4,
		// corners
		0,4,	1,5,	2,6,	3,7
	};

	static dd::ConstBuffer<uint> s_indicesBuffer(s_indices, std::size(s_indices));

	BoundsRenderer::BoundsRenderer() :
		IRenderer("Bounds")
	{
		Require<dd::TransformComponent>();
		Optional<dd::BoundBoxComponent>();
		Optional<dd::BoundSphereComponent>();
		RequireTag(ddc::Tag::Visible);

		m_renderState.BackfaceCulling = true;
		m_renderState.Blending = true;
		m_renderState.Depth = true;
	}

	BoundsRenderer::~BoundsRenderer()
	{

	}

	void BoundsRenderer::Initialize()
	{
		auto handler = [this]()
		{
			int mode = (int)m_drawMode;
			++mode;

			if (mode > 2)
				mode = 0;

			m_drawMode = (DrawMode)mode;
			m_updateBuffers = true;
		};

		s_input->AddHandler(dd::InputAction::TOGGLE_BOUNDS, handler);

		m_shader = s_shaderManager->Load("line");
		DD_ASSERT(m_shader.IsValid());

		m_vao.Create("bounds");
		m_vao.Bind();

		m_vboPosition.Create("bounds");
		m_vao.BindVBO(m_vboPosition, 0, sizeof(s_cornersBuffer[0]));
		m_shader->BindPositions(m_vao, m_vboPosition);

		m_vboIndex.Create("bounds.index");
		m_vao.BindIndices(m_vboIndex);

		m_vao.Unbind();

		m_updateBuffers = true;
	}

	void BoundsRenderer::UpdateBuffers()
	{
		ScopedShader scoped_shader = m_shader->UseScoped();

		if (m_drawMode == DrawMode::Box)
		{
			m_vboPosition.SetData(s_cornersBuffer);
			m_vboIndex.SetData(s_indicesBuffer);
		}
		else if (m_drawMode == DrawMode::Sphere)
		{
			dd::MeshUtils::MakeIcosphereLines(m_vboPosition, m_vboIndex, m_subdivisions);
		}

		m_updateBuffers = false;
	}

	void BoundsRenderer::Render(const ddr::RenderData& data)
	{
		if (m_drawMode == DrawMode::None)
		{
			return;
		}

		if (m_updateBuffers)
		{
			UpdateBuffers();
		}

		ScopedRenderState scoped_state = m_renderState.UseScoped();

		DD_ASSERT(m_shader.IsValid());

		ScopedShader scoped_shader = m_shader->UseScoped();

		const ddc::EntityLayer& entity_space = data.Layer();

		glm::mat4 view_projection = data.Camera().GetProjectionMatrix() * data.Camera().GetViewMatrix();

		auto transforms = data.Get<dd::TransformComponent>();
		auto bound_boxes = data.Get<dd::BoundBoxComponent>();
		auto bound_spheres = data.Get<dd::BoundSphereComponent>();

		const std::vector<ddc::Entity> entities = data.Entities();

		m_vao.Bind();

		for (size_t i = 0; i < data.Size(); ++i)
		{
			if (entities[i].HasTag(ddc::Tag::Selected))
			{
				scoped_shader->SetUniform("Colour", glm::vec4(0, 1, 0, 1));
			}
			else if (entities[i].HasTag(ddc::Tag::Focused))
			{
				scoped_shader->SetUniform("Colour", glm::vec4(1, 1, 0, 1));
			}
			else
			{
				scoped_shader->SetUniform("Colour", glm::vec4(1, 1, 1, 0.5));
			}

			ddm::AABB bound_box;
			ddm::Sphere bound_sphere;
			if (!GetWorldBoundBoxAndSphere(bound_boxes.Get(i), bound_spheres.Get(i), transforms[i], bound_box, bound_sphere))
			{
				continue;
			}

			glm::mat4 model;

			if (m_drawMode == DrawMode::Box)
			{
				model = glm::translate(bound_box.Min) * glm::scale(bound_box.Max - bound_box.Min);
			}
			else if (m_drawMode == DrawMode::Sphere)
			{
				model = glm::translate(bound_sphere.Centre) * glm::scale(glm::vec3(bound_sphere.Radius));
			}

			scoped_shader->SetUniform("ModelViewProjection", view_projection * model);

			OpenGL::DrawElements(ddr::Primitive::Lines, m_vboIndex.SizeBytes() / sizeof(uint));
		}

		m_vao.Unbind();
	}

	void BoundsRenderer::DrawDebugInternal()
	{
		ImGui::SetWindowSize(ImVec2(200, 120), ImGuiCond_Once);

		static const char* c_drawModes = "None\0Box\0Sphere\0";

		int drawMode = (int) m_drawMode;
		if (ImGui::Combo("Mode", &drawMode, c_drawModes))
		{
			m_drawMode = (DrawMode) drawMode;
			m_updateBuffers = true;
		}

		if (ImGui::SliderInt("Subdivisions", &m_subdivisions, 0, 6))
		{
			m_updateBuffers = true;
		}

		uint64 triangles = (m_vboIndex.SizeBytes() / sizeof(uint)) / 6;
		ImGui::Value("Triangles", triangles);
	}
}
