//
// SpriteRenderer.cpp - A sprite renderer.
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#include "PCH.h"
#include "SpriteRenderer.h" 

#include "ICamera.h"

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
		m_shader = s_shaderManager->Load("sprite");
		m_vboQuad.Create("sprite.quad");
		m_vboQuad.SetData(dd::ConstBuffer<glm::vec2>(s_quad, 6));
	}

	SpriteRenderer::InstanceVBOs& SpriteRenderer::FindCachedInstanceVBOs(ddr::SpriteHandle sprite_h)
	{
		auto it_pair = m_instanceCache.insert({ sprite_h, InstanceVBOs() });
		return it_pair.first->second;
	}

	void SpriteRenderer::DrawInstancedSprites(SpriteIterator start, SpriteIterator end, UniformStorage& uniforms, Shader& shader)
	{
		const Sprite* sprite = start->Sprite.Get();

		m_transforms.clear();
		m_colours.clear();
		m_uvOffsets.clear();
		m_uvScales.clear();

		int count = 0;
		for (SpriteIterator it = start; it != end; ++it)
		{
			m_transforms.push_back(d2d::Calculate2DTransform(it->Position, it->Size, it->Rotation, it->Pivot));
			m_colours.push_back(it->Colour);

			m_uvOffsets.push_back(sprite->OffsetNormalized);
			m_uvScales.push_back(sprite->SizeNormalized);

			++count;
		}

		InstanceVBOs& cache = FindCachedInstanceVBOs(start->Sprite);
		if (!cache.VAO.IsValid())
		{
			cache.VAO.Create("sprite");
			cache.VAO.Bind();

			shader.CreateAttributeVec2(cache.VAO, m_vboQuad, "Position", Normalized::No, Instanced::No);
			shader.CreateAttributeVec2(cache.VAO, m_vboQuad, "UV", Normalized::No, Instanced::No);

			cache.Transforms.Create("sprite.transform");
			cache.VAO.BindVBO(cache.Transforms, 0, sizeof(m_transforms[0]));
			shader.CreateAttributeMat3(cache.VAO, cache.Transforms, "TransformInstanced", Normalized::No, Instanced::Yes);

			cache.Colours.Create("sprite.colour");
			cache.VAO.BindVBO(cache.Colours, 0, sizeof(m_colours[0]));
			shader.CreateAttributeMat3(cache.VAO, cache.Colours, "ColourInstanced", Normalized::No, Instanced::Yes);

			cache.UVOffsets.Create("sprite.uv_offset");
			cache.VAO.BindVBO(cache.UVOffsets, 0, sizeof(m_uvOffsets[0]));
			shader.CreateAttributeMat3(cache.VAO, cache.UVOffsets, "UVOffsetInstanced", Normalized::No, Instanced::Yes);

			cache.UVScales.Create("sprite.uv_scale");
			cache.VAO.BindVBO(cache.UVScales, 0, sizeof(m_uvScales[0]));
			shader.CreateAttributeMat3(cache.VAO, cache.UVScales, "UVScaleInstanced", Normalized::No, Instanced::Yes);

			cache.VAO.Unbind();
		}
		
		cache.Transforms.SetData(m_transforms);
		cache.Colours.SetData(m_colours);
		cache.UVOffsets.SetData(m_transforms);
		cache.UVScales.SetData(m_uvScales);

		Texture* texture = sprite->Texture.Access();

		cache.VAO.Bind();

		texture->Bind(0);
		uniforms.Set("Texture", *texture);

		OpenGL::DrawArraysInstanced(ddr::Primitive::Triangles, 6, count);
		CheckOGLError();

		texture->Unbind();

		cache.VAO.Unbind();
	}

	void SpriteRenderer::DrawLayer(SpriteIterator start, SpriteIterator end, ddr::UniformStorage& uniforms, ddr::Shader& shader)
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

			DrawInstancedSprites(tex_start, tex_end, uniforms, shader);

			tex_start = tex_end;
		}
	}

	void SpriteRenderer::Render(const ddr::RenderData& data)
	{
		ScopedShader shader = m_shader.Access()->UseScoped();

		ddr::UniformStorage& uniforms = data.Uniforms();
		const ddr::ICamera& camera = data.Camera();
		const ddc::EntityLayer& entities = data.Layer();

		uniforms.Upload(*shader);

		ScopedRenderState scoped_state = m_renderState.UseScoped();

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

			DrawLayer(layer_start, layer_end, uniforms, *shader);

			layer_start = layer_end;
		}
	}

	void SpriteRenderer::DrawDebugInternal()
	{
		ImGui::Value("Sprites", m_temp.size());
	}
}
