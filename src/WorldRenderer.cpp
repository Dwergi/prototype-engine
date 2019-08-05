//
// WorldRenderer.cpp - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#include "PCH.h"
#include "WorldRenderer.h"

#include "AABB.h"
#include "CommandBuffer.h"
#include "GLError.h"
#include "Frustum.h"
#include "ICamera.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MousePicking.h"
#include "LightComponent.h"
#include "OpenGL.h"
#include "ParticleSystem.h"
#include "Services.h"
#include "ShaderPart.h"
#include "Shader.h"
#include "TransformComponent.h"
#include "Uniforms.h"
#include "IWindow.h"

namespace ddr
{
	static dd::Service<ddr::ShaderManager> s_shaderManager;
	static dd::Service<ddr::TextureManager> s_textureManager;
	static dd::Service<dd::IWindow> s_window;

	struct Fog
	{
		bool Enabled { true };
		float Distance { 1000.0f };
		glm::vec3 Colour { 0.6, 0.7, 0.8 };

		void UpdateUniforms(ddr::UniformStorage& uniforms) const
		{
			uniforms.Set("Fog.Enabled", Enabled);
			uniforms.Set("Fog.Distance", Distance);
			uniforms.Set("Fog.Colour", Colour);
		}
	};

	Fog s_fog;

	WorldRenderer::WorldRenderer()
	{
		m_uniforms = new ddr::UniformStorage();
		m_commands = new ddr::CommandBuffer();

		m_defaultState.Blending = false;
		m_defaultState.BackfaceCulling = true;
		m_defaultState.Depth = true;

		m_depthState.Blending = true;
		m_depthState.BackfaceCulling = false;
		m_depthState.Depth = false;
	}

	WorldRenderer::~WorldRenderer()
	{
		delete m_uniforms;
		delete m_commands;
	}

	void WorldRenderer::InitializeRenderers(ddc::EntitySpace& entities)
	{
		CreateFrameBuffer(s_window->GetSize());
		m_previousSize = s_window->GetSize();

		for (ddr::Renderer* current : m_renderers)
		{
			current->RenderInit(entities);
		}
	}

	void WorldRenderer::Register(ddr::Renderer& renderer)
	{
		m_renderers.push_back(&renderer);
	}

	void WorldRenderer::CreateFrameBuffer(glm::ivec2 size)
	{
		m_colourTexture.Initialize(size, GL_SRGB8_ALPHA8, 1);
		m_colourTexture.Create();
		m_depthTexture.Initialize(size, GL_DEPTH_COMPONENT32F, 1);
		m_depthTexture.Create();

		glm::vec4 clear_colour = glm::vec4(m_skyColour.xyz, 0.0);
		m_framebuffer.SetClearColour(glm::vec4(0));
		m_framebuffer.SetClearDepth(0.0f);
		m_framebuffer.Create(m_colourTexture, &m_depthTexture);
		m_framebuffer.RenderInit();
	}

	void WorldRenderer::DrawDebugInternal(ddc::EntitySpace& entities)
	{
		ImGui::Checkbox("Draw Depth", &m_debugDrawDepth);
		ImGui::Checkbox("Draw Normals", &m_debugDrawNormals);

		if (ImGui::TreeNodeEx("Fog", ImGuiTreeNodeFlags_CollapsingHeader))
		{
			ImGui::Checkbox("Enabled", &s_fog.Enabled);
			ImGui::SliderFloat("Distance", &s_fog.Distance, 0, 10000, "%.1f");
			ImGui::ColorEdit3("Colour", glm::value_ptr(s_fog.Colour));

			ImGui::TreePop();
		}

		if (ImGui::Button("Reload Shaders"))
		{
			m_reloadShaders = true;
		}
	}

	void WorldRenderer::RenderDebug(const ddr::RenderData& data, ddr::Renderer& debug_render)
	{
		m_framebuffer.BindDraw();

		debug_render.RenderDebug(data);

		m_framebuffer.UnbindDraw();
	}

	void WorldRenderer::CallRenderer(ddr::Renderer& renderer, ddc::EntitySpace& entities, const ddr::ICamera& camera, std::function<void(Renderer&, const RenderData&)> fn) const
	{
		dd::Array<dd::ComponentID, ddc::MAX_COMPONENTS> required;
		for (const ddc::DataRequest* req : renderer.GetRequirements())
		{
			if (!req->Optional())
			{
				required.Add(req->Component().ComponentID());
			}
		}

		const std::bitset<ddc::MAX_TAGS>& tags = renderer.GetRequiredTags();

		std::vector<ddc::Entity> entities;
		entities.FindAllWith(required, tags, entities);

		if (entities.size() == 0)
		{
			return;
		}

		RenderData data(entities, camera, *m_uniforms, *m_commands, entities, renderer.GetRequirements());

		fn(renderer, data);
	}

	void WorldRenderer::Render(ddc::EntitySpace& entities, const ddr::ICamera& camera, float delta_t)
	{
		m_time += delta_t;

		m_commands->Clear();

		for (ddr::Renderer* r : m_renderers)
		{
			r->RenderUpdate(entities);
		}

		ddr::Renderer* debug_render = nullptr;

		BeginRender(entities, camera);

		for (ddr::Renderer* r : m_renderers)
		{
			if (!r->UsesAlpha())
			{
				CallRenderer(*r, entities, camera,
					[](Renderer& rend, const RenderData& data) { rend.Render(data); });
			}

			if (r->ShouldRenderDebug())
			{
				debug_render = r;
				break;
			}
		}

		if (debug_render != nullptr)
		{
			CallRenderer(*debug_render, entities, camera,
				[](Renderer& rend, const RenderData& data) { rend.RenderDebug(data); });
		}

		for (ddr::Renderer* r : m_renderers)
		{
			if (r->UsesAlpha())
			{
				CallRenderer(*r, entities, camera,
					[](Renderer& rend, const RenderData& data) { rend.Render(data); });
			}
		}

		m_commands->Sort(camera);
		m_commands->Dispatch(*m_uniforms);

		EndRender(*m_uniforms, camera);
	}

	void WorldRenderer::BeginRender(const ddc::EntitySpace& entities, const ddr::ICamera& camera)
	{
		if (m_reloadShaders)
		{
			s_shaderManager->ReloadAll();
			m_reloadShaders = false;
		}

		if (s_window->GetSize() != m_previousSize)
		{
			m_framebuffer.Destroy();
			m_colourTexture.Destroy();
			m_depthTexture.Destroy();

			CreateFrameBuffer(s_window->GetSize());

			m_previousSize = s_window->GetSize();
		}

		m_framebuffer.BindRead();
		m_framebuffer.BindDraw();
		m_framebuffer.Clear();

		if (m_debugDrawDepth)
		{
			m_depthState.Use(true);
		}
		else
		{
			m_defaultState.Use(true);
		}

		s_fog.UpdateUniforms(*m_uniforms);

		m_uniforms->Set("ViewPosition", camera.GetPosition());
		m_uniforms->Set("View", camera.GetViewMatrix());
		m_uniforms->Set("Projection", camera.GetProjectionMatrix());
		m_uniforms->Set("DrawNormals", m_debugDrawNormals);
		m_uniforms->Set("Time", m_time);
	}

	void WorldRenderer::EndRender(ddr::UniformStorage& uniforms, const ddr::ICamera& camera)
	{
		m_framebuffer.BindRead();

		glm::ivec2 window_size = s_window->GetSize();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glViewport(0, 0, window_size.x, window_size.y);
		CheckOGLError();

		if (m_debugDrawDepth)
		{
			m_framebuffer.RenderDepth(uniforms, camera);
			m_depthState.Use(false);
		}
		else
		{
			m_framebuffer.Blit();

			m_defaultState.Use(false);
		}

		m_framebuffer.UnbindRead();
	}
}