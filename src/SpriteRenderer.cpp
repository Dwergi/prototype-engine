//
// SpriteRenderer.cpp - A sprite renderer.
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#include "PCH.h"
#include "SpriteRenderer.h" 

#include "ICamera.h"
#include "OpenGL.h"
#include "RenderData.h"
#include "Services.h"
#include "ShaderPart.h"
#include "Shader.h"
#include "Uniforms.h"

#include "d2d/SpriteComponent.h"

namespace ddr
{
	static dd::Service<ddr::ShaderManager> s_shaderManager;

	static const glm::vec2 s_quad[] = {
			glm::vec2(0,	0),
			glm::vec2(1,	0),
			glm::vec2(0,	1),
			glm::vec2(0,	1),
			glm::vec2(1,	0),
			glm::vec2(1,	1)
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

			shader->BindAttributeVec2("Position", false);
			shader->BindAttributeVec2("UV", false);
			s_vboQuad.Unbind();
		}


		m_vboPositions.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		{
			m_vboPositions.Bind();
			shader->BindAttributeVec2("PositionInstanced", false);
			shader->SetAttributeInstanced("PositionInstanced");
			m_vboPositions.Unbind();
		}

		m_vboSizes.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		{
			m_vboSizes.Bind();
			shader->BindAttributeVec2("SizeInstanced", false);
			shader->SetAttributeInstanced("SizeInstanced");
			m_vboSizes.Unbind();
		}

		m_vboColours.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		{
			m_vboColours.Bind();
			shader->BindAttributeVec4("ColourInstanced", false);
			shader->SetAttributeInstanced("ColourInstanced");
			m_vboColours.Unbind();
		}

		m_vboUVOffsets.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		{
			m_vboUVOffsets.Bind();
			shader->BindAttributeVec2("UVOffsetInstanced", false);
			shader->SetAttributeInstanced("UVOffsetInstanced");
			m_vboUVOffsets.Unbind();
		}

		m_vboUVScales.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		{
			m_vboUVScales.Bind();
			shader->BindAttributeVec2("UVScaleInstanced", false);
			shader->SetAttributeInstanced("UVScaleInstanced");
			m_vboUVScales.Unbind();
		}

		m_vao.Unbind();
	}

	void SpriteRenderer::DrawInstancedSprites(SpriteIterator start, SpriteIterator end, ddr::UniformStorage& uniforms)
	{
		const ddr::Sprite* sprite = start->Sprite.Get();
		ddr::TextureHandle texture_h = sprite->Texture;
		ddr::Texture* texture = texture_h.Access();
		glm::vec2 tex_size = texture->GetSize();

		glActiveTexture(GL_TEXTURE0);
		texture->Bind(0);
		uniforms.Set("Texture", *texture);

		size_t count = std::distance(start, end);

		m_positions.clear();
		m_sizes.clear();
		m_uvOffsets.clear();
		m_uvScales.clear();
		m_colours.clear();

		for (SpriteIterator it = start; it != end; ++it)
		{
			const ddr::Sprite* sprite = it->Sprite.Get();
			DD_ASSERT(sprite->Texture == texture_h);

			m_positions.push_back(it->Position);
			m_sizes.push_back(it->Size);
			m_colours.push_back(it->Colour);

			m_uvOffsets.push_back(sprite->OffsetNormalized);
			m_uvScales.push_back(sprite->SizeNormalized);
		}

		m_vboPositions.Bind();
		m_vboPositions.CommitData();
		m_vboPositions.Unbind();

		m_vboSizes.Bind();
		m_vboSizes.CommitData();
		m_vboSizes.Unbind();

		m_vboUVOffsets.Bind();
		m_vboUVOffsets.CommitData();
		m_vboUVOffsets.Unbind();

		m_vboUVScales.Bind();
		m_vboUVScales.CommitData();
		m_vboUVScales.Unbind();

		m_vboColours.Bind();
		m_vboColours.CommitData();
		m_vboColours.Unbind();

		OpenGL::DrawArraysInstanced(6, (int) count);
		CheckOGLError();

		texture->Unbind();
	}

	void SpriteRenderer::DrawLayer(SpriteIterator start, SpriteIterator end, ddr::UniformStorage& uniforms)
	{
		// sort by texture to get instanceable groups
		std::sort(start, end, [](auto& a, auto& b)
		{
			const ddr::Sprite* a_sprite = a.Sprite.Get();
			const ddr::Sprite* b_sprite = b.Sprite.Get();
			return a_sprite->Texture < b_sprite->Texture;
		});

		SpriteIterator tex_start = start;
		ddr::TextureHandle current_tex = tex_start->Sprite.Get()->Texture;

		while (tex_start < end)
		{
			SpriteIterator tex_end = std::find_if_not(tex_start, end, [current_tex](auto& a)
			{
				return a.Sprite.Get()->Texture == current_tex;
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
		const ddc::EntityLayer& entities = data.EntityLayer();

		uniforms.Bind(*shader);

		ScopedRenderState scoped_state = m_renderState.UseScoped();

		m_vao.Bind();

		auto sprite_cmps = data.Get<d2d::SpriteComponent>();
		
		// copy components to temp
		m_temp.clear();
		std::copy(sprite_cmps.begin(), sprite_cmps.end(), std::back_inserter(m_temp));

		// reserve memory
		m_positions.reserve(m_temp.size());
		m_sizes.reserve(m_temp.size());
		m_uvOffsets.reserve(m_temp.size());
		m_uvScales.reserve(m_temp.size());
		m_colours.reserve(m_temp.size());

		m_vboPositions.Bind();
		m_vboPositions.SetData(m_positions.data(), m_positions.capacity());
		m_vboPositions.Unbind();

		m_vboSizes.Bind();
		m_vboSizes.SetData(m_sizes.data(), m_sizes.capacity());
		m_vboSizes.Unbind();

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
		std::sort(m_temp.begin(), m_temp.end(), [](auto& a, auto& b)
		{
			return a.ZIndex < b.ZIndex;
		});

		SpriteIterator layer_start = m_temp.begin();
		while (layer_start < m_temp.end())
		{
			// find last index of the same layer
			const int layer_z_index = layer_start->ZIndex;
			SpriteIterator layer_end = std::find_if_not(layer_start, m_temp.end(), [layer_z_index](auto& a)
			{
				return a.ZIndex == layer_z_index;
			});

			DrawLayer(layer_start, layer_end, uniforms);

			layer_start = layer_end;
		}

		m_vao.Unbind();
		
		uniforms.Unbind();
	}
}