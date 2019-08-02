#pragma once

namespace sf
{
	class SoundSource;
}

namespace ddc
{
	struct SoundComponent
	{
		sfml::SoundSource* SoundSource;

		DD_BEGIN_CLASS(dd::SoundComponent)
			DD_MEMBER()
	};
}