#pragma once

#include "IInputSource.h"

namespace sf
{
	class Window;
}

namespace dd
{
	struct SFMLInputSource : dd::IInputSource
	{
	private:
		virtual void OnUpdateInput() override;
		virtual void OnSetMouseCapture(bool capture) override;

		sf::Window* m_sfmlWindow { nullptr };
	};
}