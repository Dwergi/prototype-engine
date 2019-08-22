//
// RenderManager.cpp - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#include "PCH.h"
#include "RenderManager.h"

#include "AABB.h"
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

	RenderManager::RenderManager()
	{
		DD_TODO("Rename this");
		m_defaultState.Blending = false;
		m_defaultState.BackfaceCulling = true;
		m_defaultState.Depth = true;

		m_depthState.Blending = true;
		m_depthState.BackfaceCulling = false;
		m_depthState.Depth = false;
	}

	RenderManager::~RenderManager()
	{
	}

	void RenderManager::Initialize()
	{
		CreateFrameBuffer(s_window->GetSize());
		m_previousSize = s_window->GetSize();

		for (ddr::IRenderer* current : m_renderers)
		{
			current->Initialize();
		}
	}

	void RenderManager::Shutdown()
	{
		for (ddr::IRenderer* current : m_renderers)
		{
			current->Shutdown();
		}
	}

	void RenderManager::Register(ddr::IRenderer& renderer)
	{
		m_renderers.push_back(&renderer);
	}

	void RenderManager::CreateFrameBuffer(glm::ivec2 size)
	{
		m_colourTexture.Initialize(size, GL_SRGB8_ALPHA8, 1);
		m_colourTexture.Create();
		m_depthTexture.Initialize(size, GL_DEPTH_COMPONENT32F, 1);
		m_depthTexture.Create();

		glm::vec4 clear_colour = glm::vec4(m_skyColour.xyz, 0.0);
		m_framebuffer.SetClearColour(glm::vec4(0));
		m_framebuffer.SetClearDepth(0.0f);
		m_framebuffer.Create(m_colourTexture, &m_depthTexture);
		m_framebuffer.Initialize();
	}

	void RenderManager::DrawDebugInternal()
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

	void RenderManager::RenderDebug(const ddr::RenderData& data, ddr::IRenderer& debug_render)
	{
		m_framebuffer.BindDraw();

		debug_render.RenderDebug(data);

		m_framebuffer.UnbindDraw();
	}

	void RenderManager::FillRenderData(ddr::IRenderer& renderer, ddc::EntityLayer& layer, const ddr::ICamera& camera, float delta_t)
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
		layer.FindAllWith(required, tags, entities);

		renderer.AccessRenderData().Fill(layer, camera, m_uniforms, std::move(entities), renderer.GetRequirements(), delta_t);
	}

	void RenderManager::Render(ddc::EntityLayer& layer, const ddr::ICamera& camera, float delta_t)
	{
		m_time += delta_t;

		// create render data
		for (size_t i = 0; i < m_renderers.size(); ++i)
		{
			FillRenderData(*m_renderers[i], layer, camera, delta_t);
		}

		// update
		for (size_t i = 0; i < m_renderers.size(); ++i)
		{
			m_renderers[i]->Update(*render_data[i]);
		}

		ddr::IRenderer* debug_render = nullptr;

		BeginRender(layer, camera);

		// render non-alpha
		for (size_t i = 0; i < m_renderers.size(); ++i)
		{
			ddr::IRenderer& renderer = *m_renderers[i];
			if (!renderer.UsesAlpha())
			{
				renderer.Render(renderer.AccessRenderData());
			}
		}

		// render debug
		for (size_t i = 0; i < m_renderers.size(); ++i)
		{
			ddr::IRenderer& renderer = *m_renderers[i];
			if (renderer.ShouldRenderDebug())
			{
				renderer.RenderDebug(renderer.AccessRenderData());
			}
		}

		// render alpha
		for (size_t i = 0; i < m_renderers.size(); ++i)
		{
			ddr::IRenderer& renderer = *m_renderers[i];
			if (renderer.UsesAlpha())
			{
				renderer.Render(renderer.AccessRenderData());
			}
		}

		EndRender(m_uniforms, camera);
	}

	void RenderManager::BeginRender(const ddc::EntityLayer& layer, const ddr::ICamera& camera)
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

		s_fog.UpdateUniforms(m_uniforms);

		m_uniforms.Set("ViewPosition", camera.GetPosition());
		m_uniforms.Set("View", camera.GetViewMatrix());
		m_uniforms.Set("Projection", camera.GetProjectionMatrix());
		m_uniforms.Set("DrawNormals", m_debugDrawNormals);
		m_uniforms.Set("Time", m_time);
	}

	void RenderManager::EndRender(ddr::UniformStorage& uniforms, const ddr::ICamera& camera)
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