#include "PCH.h"
#include "SFMLWindow.h"

#include "sfml/Window.hpp"

namespace dd
{
	using super = IWindow;

	bool SFMLWindow::OnInitialize()
	{
		DD_ASSERT(m_sfmlWindow == nullptr);

		glm::ivec2 size = super::GetSize();
		DD_ASSERT(size != glm::ivec2(0, 0));

		std::string title = super::GetTitle();
		DD_ASSERT(!title.empty());

		sf::VideoMode video_mode(size.x, size.y);
		sf::String sfml_title = title;

		m_sfmlWindow = new sf::Window(video_mode, sfml_title);

		return true;
	}

	void SFMLWindow::OnShutdown()
	{
		if (m_sfmlWindow == nullptr)
		{
			return;
		}

		m_sfmlWindow->close();
		delete m_sfmlWindow;
	}

	void SFMLWindow::OnSetSize(glm::ivec2 size)
	{
		if (m_sfmlWindow == nullptr)
		{
			return;
		}

		sf::Vector2u sfml_size(size.x, size.y);
		m_sfmlWindow->setSize(sfml_size);
	}

	void SFMLWindow::OnSetTitle(std::string title)
	{
		if (m_sfmlWindow == nullptr)
		{
			return;
		}

		sf::String sfml_title = title;
		m_sfmlWindow->setTitle(title);
	}

	void SFMLWindow::OnSetBorderless(bool borderless)
	{
		if (m_sfmlWindow == nullptr)
		{
			return;
		}

		sf::VideoMode video_mode = sf::VideoMode::getDesktopMode();
		m_sfmlWindow->setSize(sf::Vector2u(video_mode.width, video_mode.height));
	}

	void SFMLWindow::OnSwap()
	{
		if (m_sfmlWindow == nullptr)
		{
			return;
		}

		m_sfmlWindow->display();
	}

	bool SFMLWindow::OnGetFocused()
	{
		if (m_sfmlWindow == nullptr)
		{
			return false;
		}

		return m_sfmlWindow->hasFocus();
	}
}