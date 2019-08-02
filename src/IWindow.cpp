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
		m_size = size;
		OnSetSize(size);
		return *this;
	}

	IWindow& IWindow::SetTitle(std::string title)
	{
		m_title = title;
		OnSetTitle(title);
		return *this;
	}
}