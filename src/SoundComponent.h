#pragma once

namespace sf
{
	class SoundSource;
}

namespace ddc
{
	struct SoundComponent
	{
		sf::SoundSource* SoundSource;

		DD_BEGIN_CLASS(dd::SoundComponent)
			DD_MEMBER()
		DD_END_CLASS()
	};
}