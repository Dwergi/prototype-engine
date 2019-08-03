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
		SFMLInputSource();

	private:
		virtual void OnUpdateInput() override;
		virtual void OnSetMouseCapture(bool capture) override;
		virtual void OnSetMousePosition(glm::vec2 pos) override;

		virtual const char* GetClipboardText() const override;
		virtual void SetClipboardText(const char* text) override;

		sf::Window* m_sfmlWindow { nullptr };
	};
}