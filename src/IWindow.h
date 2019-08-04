#pragma once

namespace dd
{
	enum class Cursor : int8
	{
		Hidden = -1,
		Arrow = 0,
		Text,
		SizeAll,
		SizeNS,
		SizeEW,
		SizeNESW,
		SizeNWSE,
		Hand,
		COUNT
	};

	struct IWindow
	{
		bool Initialize();
		void Shutdown();

		void Update(float delta_t);

		IWindow& Swap();

		bool IsClosing() const { return m_closing; }
		IWindow& SetToClose();

		bool IsFocused() const { return m_focused; }

		IWindow& SetBorderless(bool borderless);

		dd::Cursor GetCursor() const;
		IWindow& SetCursor(dd::Cursor cursor);

		IWindow& SetSize(glm::ivec2 size);
		glm::ivec2 GetSize() const { return m_size; }

		IWindow& SetTitle(std::string title);
		std::string GetTitle() const { return m_title; }

		float GetAspectRatio() const { return float(m_size.x) / float(m_size.y); }

		virtual void* GetNative() const = 0;

	private:
		virtual bool OnInitialize() = 0;
		virtual void OnUpdate(float delta_t) = 0;
		virtual void OnShutdown() = 0;

		virtual void OnSetSize(glm::ivec2 size) = 0;
		virtual void OnSetTitle(std::string title) = 0;
		virtual void OnSetBorderless(bool borderless) = 0;
		virtual void OnSetMousePosition(glm::ivec2 pos) = 0;
		virtual void OnSwap() = 0;
		virtual bool OnGetFocused() = 0;
		virtual void OnSetCursor(Cursor cursor) = 0;

		glm::ivec2 m_size { 0, 0 };
		std::string m_title;
		bool m_focused { false };
		bool m_closing { false };
		dd::Cursor m_cursor { Cursor::Hidden };
	};
}