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

	struct Light2DComponent
	{
		LightType Type { LightType::None };

		DD_BEGIN_CLASS(lux::Light2DComponent)
			DD_COMPONENT();
		DD_END_CLASS()
	};
}