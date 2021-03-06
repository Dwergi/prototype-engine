//
// SpriteRenderer.cpp - A sprite renderer.
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#include "PCH.h"
#include "SpriteRenderer.h" 

#include "ICamera.h"
#include "Services.h"

#include "ddr/OpenGL.h"
#include "ddr/RenderData.h"
#include "ddr/ShaderPart.h"
#include "ddr/Shader.h"
#include "ddr/Uniforms.h"

#include "d2d/SpriteComponent.h"
#include "d2d/Transform2DComponent.h"

#include <iterator>

namespace ddr
{
	static dd::Service<ddr::ShaderManager> s_shaderManager;

	static const glm::vec2 s_quad[] = {
			glm::vec2(0, 0),
			glm::vec2(1, 0),
			glm::vec2(0, 1),
			glm::vec2(0, 1),
			glm::vec2(1, 0),
			glm::vec2(1, 1)
	};

	static VBO s_vboQuad;

	static ShaderHandle s_shader;

	SpriteRenderer::SpriteRenderer() :
		ddr::IRenderer("Sprites")
	{
		Require<d2d::SpriteComponent>();
		RequireTag(ddc::Tag::Visible);

		m_renderState.BackfaceCulling = false;
		m_renderState.Blending = true;
		m_renderState.Depth = false;
		m_renderState.DepthWrite = false;
	}

	void SpriteRenderer::Initialize()
	{
		s_shader = s_shaderManager->Load("sprite");

		Shader* shader = s_shader.Access();
		DD_ASSERT(shader != nullptr);

		ScopedShader scoped_state = shader->UseScoped();

		m_vao.Create();
		m_vao.Bind();

		s_vboQuad.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		{
			s_vboQuad.Bind();
			s_vboQuad.SetData(dd::ConstBuffer<glm::vec2>(s_quad, 6));
			s_vboQuad.CommitData();

			shader->BindAttributeVec2("Position");
			shader->BindAttributeVec2("UV");
			s_vboQuad.Unbind();
		}

		m_vboTransforms.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		{
			m_vboTransforms.Bind();
			shader->BindAttributeMat3("TransformInstanced", Normalized::No, Instanced::Yes);
			m_vboTransforms.Unbind();
		}

		m_vboColours.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		{
			m_vboColours.Bind();
			shader->BindAttributeVec4("ColourInstanced", Normalized::No, Instanced::Yes);
			m_vboColours.Unbind();
		}

		m_vboUVOffsets.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		{
			m_vboUVOffsets.Bind();
			shader->BindAttributeVec2("UVOffsetInstanced", Normalized::No, Instanced::Yes);
			m_vboUVOffsets.Unbind();
		}

		m_vboUVScales.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		{
			m_vboUVScales.Bind();
			shader->BindAttributeVec2("UVScaleInstanced", Normalized::No, Instanced::Yes);
			m_vboUVScales.Unbind();
		}

		m_vao.Unbind();
	}

	DD_OPTIMIZE_OFF()

	void SpriteRenderer::DrawInstancedSprites(SpriteIterator start, SpriteIterator end, ddr::UniformStorage& uniforms)
	{
		const ddr::Sprite* sprite = start->Sprite.Get();
		ddr::TextureHandle texture_h = sprite->Texture;
		ddr::Texture* texture = texture_h.Access();
		glm::vec2 tex_size = texture->GetSize();

		glActiveTexture(GL_TEXTURE0);
		texture->Bind(0);
		uniforms.Set("Texture", *texture);

		m_transforms.clear();
		m_uvOffsets.clear();
		m_uvScales.clear();
		m_colours.clear();

		int count = 0;

		for (SpriteIterator it = start; it != end; ++it)
		{
			const ddr::Sprite* sprite = it->Sprite.Get();
			DD_ASSERT(sprite->Texture == texture_h);

			m_transforms.push_back(d2d::Calculate2DTransform(it->Position, it->Size, it->Rotation, it->Pivot));
			m_colours.push_back(it->Colour);

			m_uvOffsets.push_back(sprite->OffsetNormalized);
			m_uvScales.push_back(sprite->SizeNormalized);

			++count;
		}

		m_vboTransforms.Bind();
		m_vboTransforms.CommitData();
		m_vboTransforms.Unbind();

		m_vboUVOffsets.Bind();
		m_vboUVOffsets.CommitData();
		m_vboUVOffsets.Unbind();

		m_vboUVScales.Bind();
		m_vboUVScales.CommitData();
		m_vboUVScales.Unbind();

		m_vboColours.Bind();
		m_vboColours.CommitData();
		m_vboColours.Unbind();

		OpenGL::DrawArraysInstanced(6, count);
		CheckOGLError();

		texture->Unbind();
	}

	void SpriteRenderer::DrawLayer(SpriteIterator start, SpriteIterator end, ddr::UniformStorage& uniforms)
	{
		// sort by texture to get instanceable groups
		std::sort(start, end, [](const auto& a, const auto& b)
		{
			return a.Sprite->Texture < b.Sprite->Texture;
		});

		SpriteIterator tex_start = start;
		while (tex_start < end)
		{
			ddr::TextureHandle current_tex = tex_start->Sprite->Texture;

			SpriteIterator tex_end = std::find_if_not(tex_start, end, 
				[current_tex](const auto& x)
				{
					return x.Sprite->Texture == current_tex;
				});

			DrawInstancedSprites(tex_start, tex_end, uniforms);

			tex_start = tex_end;
		}
	}

	void SpriteRenderer::Render(const ddr::RenderData& data)
	{
		Shader* shader = s_shader.Access();
		ScopedShader scoped_shader = shader->UseScoped();

		ddr::UniformStorage& uniforms = data.Uniforms();
		const ddr::ICamera& camera = data.Camera();
		const ddc::EntityLayer& entities = data.Layer();

		uniforms.Bind(*shader);

		ScopedRenderState scoped_state = m_renderState.UseScoped();

		m_vao.Bind();

		auto sprite_cmps = data.Get<d2d::SpriteComponent>();
		
		// copy components to temp
		m_temp.clear();
		m_temp.reserve(sprite_cmps.Size());
		std::copy(sprite_cmps.begin(), sprite_cmps.end(), std::back_inserter(m_temp));

		// reserve memory
		m_transforms.reserve(m_temp.size());
		m_uvOffsets.reserve(m_temp.size());
		m_uvScales.reserve(m_temp.size());
		m_colours.reserve(m_temp.size());

		m_vboTransforms.Bind();
		m_vboTransforms.SetData(m_transforms.data(), m_transforms.capacity());
		m_vboTransforms.Unbind();

		m_vboUVOffsets.Bind();
		m_vboUVOffsets.SetData(m_uvOffsets.data(), m_uvOffsets.capacity());
		m_vboUVOffsets.Unbind();

		m_vboUVScales.Bind();
		m_vboUVScales.SetData(m_uvScales.data(), m_uvScales.capacity());
		m_vboUVScales.Unbind();

		m_vboColours.Bind();
		m_vboColours.SetData(m_colours.data(), m_colours.capacity());
		m_vboColours.Unbind();

		// sort by z index
		std::sort(m_temp.begin(), m_temp.end(), [](const auto& a, const auto& b)
		{
			return a.ZIndex < b.ZIndex;
		});

		SpriteIterator layer_start = m_temp.begin();
		while (layer_start < m_temp.end())
		{
			// find last index of the same layer
			const int layer_z_index = layer_start->ZIndex;
			SpriteIterator layer_end = std::find_if_not(layer_start, m_temp.end(), 
				[layer_z_index](const auto& x)
				{
					return x.ZIndex == layer_z_index;
				});

			DrawLayer(layer_start, layer_end, uniforms);

			layer_start = layer_end;
		}

		m_vao.Unbind();
		
		uniforms.Unbind();
	}

	void SpriteRenderer::DrawDebugInternal()
	{
		ImGui::Value("Sprites", m_temp.size());
	}
}