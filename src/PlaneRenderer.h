//
// PlaneRenderer.h - Renderer for PlaneComponent.
// Copyright (C) Sebastian Nordgren 
// March 10th 2019
//

#pragma once

#include "IRenderer.h"

#include "ddr/Material.h"
#include "ddr/Mesh.h"
#include "ddr/Shader.h"
#include "ddr/VAO.h"
#include "ddr/VBO.h"

namespace ddr
{
	struct PlaneRenderer : IRenderer
	{
		PlaneRenderer();
		~PlaneRenderer();

		virtual void Initialize() override;
		virtual void Render(const ddr::RenderData& data) override;
		virtual bool UsesAlpha() const { return true; }

	private:

		RenderState m_lineState;
		ddr::ShaderHandle m_lineShader;
		VAO m_vao;
		VBO m_vbo;

		MeshHandle m_mesh;
		MaterialHandle m_meshMaterial;

		float m_scale { 100 };
	};
}