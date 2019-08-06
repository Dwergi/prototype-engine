#pragma once

namespace lux
{
	enum class LightType
	{
		None = -1,
		Teleporter = 1,
		Player,
		Exit,
		Yellow,
		Red
	};

	struct LuxLightComponent
	{
		LightType Type { LightType::None };

		DD_BEGIN_CLASS(lux::LuxLightComponent)
			DD_COMPONENT();
		DD_END_CLASS()
	};
}