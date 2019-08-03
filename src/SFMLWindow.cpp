#include "PCH.h"
#include "SFMLWindow.h"

#include <sfml/Window.hpp>

namespace dd
{
	using super = IWindow;

	static sf::Cursor* s_cursors[(int) dd::Cursor::COUNT];

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

		s_cursors[(int) dd::Cursor::Arrow] = new sf::Cursor();
		s_cursors[(int) dd::Cursor::Arrow]->loadFromSystem(sf::Cursor::Arrow);

		s_cursors[(int) dd::Cursor::Text] = new sf::Cursor();
		s_cursors[(int) dd::Cursor::Text]->loadFromSystem(sf::Cursor::Text);

		s_cursors[(int) dd::Cursor::SizeAll] = new sf::Cursor();
		s_cursors[(int) dd::Cursor::SizeAll]->loadFromSystem(sf::Cursor::SizeAll);

		s_cursors[(int) dd::Cursor::SizeNS] = new sf::Cursor();
		s_cursors[(int) dd::Cursor::SizeNS]->loadFromSystem(sf::Cursor::SizeVertical);

		s_cursors[(int) dd::Cursor::SizeEW] = new sf::Cursor();
		s_cursors[(int) dd::Cursor::SizeEW]->loadFromSystem(sf::Cursor::SizeHorizontal);

		s_cursors[(int) dd::Cursor::SizeNESW] = new sf::Cursor();
		s_cursors[(int) dd::Cursor::SizeNESW]->loadFromSystem(sf::Cursor::SizeBottomLeftTopRight);

		s_cursors[(int) dd::Cursor::SizeNWSE] = new sf::Cursor();
		s_cursors[(int) dd::Cursor::SizeNWSE]->loadFromSystem(sf::Cursor::SizeTopLeftBottomRight);

		s_cursors[(int) dd::Cursor::Hand] = new sf::Cursor();
		s_cursors[(int) dd::Cursor::Hand]->loadFromSystem(sf::Cursor::Hand);


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

	void SFMLWindow::OnSetMousePosition(glm::ivec2 pos)
	{
		sf::Mouse::setPosition(sf::Vector2i(pos.x, pos.y));
	}

	void SFMLWindow::OnSetCursor(dd::Cursor cursor)
	{
		if (cursor == dd::Cursor::Hidden)
		{
			m_sfmlWindow->setMouseCursorVisible(false);
		}
		else
		{
			m_sfmlWindow->setMouseCursorVisible(true);
			m_sfmlWindow->setMouseCursor(*s_cursors[(int) cursor]);
		}
	}
}