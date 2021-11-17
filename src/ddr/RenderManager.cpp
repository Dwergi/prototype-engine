//
// RenderManager.cpp - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#include "PCH.h"
#include "RenderManager.h"

#include "AssetManager.h"
#include "Frustum.h"
#include "ICamera.h"
#include "IWindow.h"
#include "LightComponent.h"
#include "MeshComponent.h"
#include "MousePicking.h"
#include "ParticleSystem.h"
#include "TransformComponent.h"

#include "ddm/AABB.h"

#include "ddr/GLError.h"
#include "ddr/Mesh.h"
#include "ddr/Material.h"
#include "ddr/OpenGL.h"
#include "ddr/Shader.h"

namespace ddr
{
	static dd::Service<dd::AssetManager> s_assetManager;
	static dd::Service<ddr::ShaderManager> s_shaderManager;
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

		dd::Services::Register(new ddr::ShaderManager());
		s_assetManager->Register(s_shaderManager.Get());

		ddr::MaterialManager* material_manager = new ddr::MaterialManager();
		dd::Services::Register(material_manager);
		s_assetManager->Register(*material_manager);

		ddr::MeshManager* mesh_manager = new ddr::MeshManager();
		dd::Services::Register(mesh_manager);
		s_assetManager->Register(*mesh_manager);
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

	void RenderManager::SetCamera(const ddr::ICamera& camera)
	{
		m_camera = &camera;
	}

	void RenderManager::Shutdown()
	{
		for (ddr::IRenderer* current : m_renderers)
		{
			current->Shutdown();
		}

		m_framebuffer.Destroy();
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

		DD_TODO("Fog isn't really relevant to all renderers...");
		if (ImGui::CollapsingHeader("Fog"))
		{
			ImGui::Checkbox("Enabled", &s_fog.Enabled);
			ImGui::SliderFloat("Distance", &s_fog.Distance, 0, 10000, "%.1f");
			ImGui::ColorEdit3("Colour", glm::value_ptr(s_fog.Colour));
		}

		DD_TODO("Shaders should probably live in their own debug view.");
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

	void RenderManager::Render(ddc::EntityLayer& layer, float delta_t)
	{
		DD_ASSERT(m_camera != nullptr, "No camera has been set through RenderManager::SetCamera!");

		m_time += delta_t;

		// create render data
		for (ddr::IRenderer* renderer : m_renderers)
		{
			renderer->RenderData().Fill(layer, *m_camera, m_uniforms, delta_t);
		}

		// update
		for (ddr::IRenderer* renderer : m_renderers)
		{
			renderer->Update(renderer->RenderData());
		}

		ddr::IRenderer* debug_render = nullptr;

		BeginRender(layer, *m_camera);

		// render non-alpha
		for (ddr::IRenderer* renderer : m_renderers)
		{
			if (!renderer->UsesAlpha())
			{
				renderer->Render(renderer->RenderData());
			}
		}

		// render debug
		for (ddr::IRenderer* renderer : m_renderers)
		{
			if (renderer->ShouldRenderDebug())
			{
				renderer->RenderDebug(renderer->RenderData());
			}
		}

		// render alpha
		for (ddr::IRenderer* renderer : m_renderers)
		{
			if (renderer->UsesAlpha())
			{
				renderer->Render(renderer->RenderData());
			}
		}

		EndRender(m_uniforms, *m_camera);
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