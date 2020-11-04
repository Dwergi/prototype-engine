#include "PCH.h"
#include "lux/LuxLightRenderer.h"

#include "Timer.h"

#include "lux/LuxLightComponent.h"

#include "d2d/SpriteComponent.h"
#include "d2d/SpriteTileSystem.h"

#include "ddr/Shader.h"

#include <fmt/format.h>

namespace lux
{
	static dd::Timer s_timer;

	LuxLightRenderer::LuxLightRenderer() :
		IRenderer("2D Lights")
	{
		RequireTag(ddc::Tag::Visible);
		Require<lux::LuxLightComponent>();
		Require<d2d::SpriteComponent>();
		s_timer.Start();
	}

	void LuxLightRenderer::Initialize()
	{
		m_shader = ddr::ShaderHandle("sprite");
	}

	void LuxLightRenderer::Render(const ddr::RenderData& render_data)
	{
		auto lights = render_data.Get<lux::LuxLightComponent>(); 
		auto sprites = render_data.Get<d2d::SpriteComponent>();

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
		uniforms.Set("Time", s_timer.TimeInSeconds());
	}
}