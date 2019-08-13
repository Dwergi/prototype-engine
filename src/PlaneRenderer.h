//
// PlaneRenderer.h - Renderer for PlaneComponent.
// Copyright (C) Sebastian Nordgren 
// March 10th 2019
//

#pragma once

#include "Material.h"
#include "Mesh.h"
#include "IRenderer.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"

namespace ddr
{
	struct PlaneRenderer : IRenderer
	{
		PlaneRenderer();
		~PlaneRenderer();

		virtual void RenderInit( ddc::EntitySpace& entities ) override;
		virtual void Render( const ddr::RenderData& data ) override;
		virtual bool UsesAlpha() const { return true; }

	private:

		RenderState m_lineState;
		ShaderHandle m_lineShader;
		VAO m_vao;
		VBO m_vbo;

		MeshHandle m_mesh;
		MaterialHandle m_meshMaterial;

		float m_scale { 100 };
	};
}