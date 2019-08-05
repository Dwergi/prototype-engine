#include "PCH.h"
#include "Light2DRenderer.h"

#include "Light2DComponent.h"
#include "Shader.h"
#include "SpriteComponent.h"
#include "SpriteTileSystem.h"
#include "Timer.h"

#include <fmt/format.h>

DD_TYPE_CPP(lux::Light2DComponent);

namespace d2d
{
	static dd::Timer s_timer;

	LightRenderer::LightRenderer() :
		Renderer("2D Lights")
	{
		RequireTag(ddc::Tag::Visible);
		Require<lux::Light2DComponent>();
		Require<dd::SpriteComponent>();
		s_timer.Start();
	}

	void LightRenderer::RenderInit(ddc::EntitySpace& entities)
	{
		m_shader = ddr::ShaderHandle("sprite");
	}

	void LightRenderer::Render(const ddr::RenderData& render_data)
	{
		auto lights = render_data.Get<lux::Light2DComponent>(); 
		auto sprites = render_data.Get<dd::SpriteComponent>();

		ddr::UniformStorage& uniforms = render_data.Uniforms();

		for (int i = 0; i < lights.Size(); ++i)
		{
			DD_ASSERT(lights[i].Type != lux::LightType::None);

			std::string intensity_name = fmt::format("Lights[{}].Type", i);
			uniforms.Set(intensity_name, (int) lights[i].Type);

			std::string position_name = fmt::format("Lights[{}].Position", i);
			uniforms.Set(position_name, sprites[i].Position + sprites[i].Size / 2.0f);
		}

		uniforms.Set("LightCount", (int) lights.Size());
		uniforms.Set("Time", s_timer.Time());
	}
}