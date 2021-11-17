#include "PCH.h"
#include "IWindow.h"

namespace dd
{
	bool IWindow::Initialize()
	{
		return OnInitialize();
	}

	void IWindow::Shutdown()
	{
		OnShutdown();
	}

	void IWindow::Update(float delta_t)
	{
		OnUpdate(delta_t);
	}

	IWindow& IWindow::SetToClose()
	{
		m_closing = true;
		return *this;
	}

	IWindow& IWindow::SetBorderless(bool borderless)
	{
		OnSetBorderless(borderless);
		return *this;
	}

	IWindow& IWindow::Swap()
	{
		OnSwap();
		m_focused = OnGetFocused();
		return *this;
	}

	IWindow& IWindow::SetSize(glm::ivec2 size)
	{
		if (m_size != size)
		{
			m_size = size;
			OnSetSize(size);
		}
		return *this;
	}

	IWindow& IWindow::SetTitle(std::string_view title)
	{
		if (m_title != title)
		{
			m_title = title;
			OnSetTitle(m_title);
		}
		return *this;
	}

	dd::Cursor IWindow::GetCursor() const
	{
		return m_cursor;
	}

	IWindow& IWindow::SetCursor(dd::Cursor cursor)
	{
		m_cursor = cursor;
		OnSetCursor(cursor);
		return *this;
	}
}