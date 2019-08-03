#pragma once

#include "IWindow.h"

namespace sf
{
	class Window;
}

namespace dd
{
	struct SFMLWindow : IWindow
	{
	private:
		virtual bool OnInitialize() override;
		virtual void OnShutdown() override;
		virtual void OnSetSize(glm::ivec2 size) override;
		virtual void OnSetTitle(std::string title) override;
		virtual void OnSetBorderless(bool borderless) override;
		virtual void OnSetCursor(dd::Cursor cursor) override;
		virtual void OnSwap() override;
		virtual bool OnGetFocused() override;
		virtual void OnSetMousePosition(glm::ivec2 pos) override;

		virtual void* GetNative() const override { return m_sfmlWindow; }

		sf::Window* m_sfmlWindow { nullptr };

	};
}