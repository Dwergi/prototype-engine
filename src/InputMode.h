//
// InputMode.h
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#pragma once

namespace dd
{
	struct InputMode
	{
		enum Predefined : int16
		{
			NONE = 0,
			GAME = 1 << 0,
			DEBUG = 1 << 1,
			ALL = (int16) 0xFF
		};

		static const int MAX_MODES = 16;

		static InputMode& Create(std::string name);
		static InputMode* Find(std::string name);
		static InputMode* Access(int16 id);

		int16 ID() const { return 1 << m_index; }

		InputMode& ShowCursor(bool show_cursor) { m_cursor = show_cursor; return *this; }
		bool ShouldShowCursor() const { return m_cursor; }

		InputMode& CaptureMouse(bool capture_mouse) { m_capture = capture_mouse; return *this; }
		bool ShouldCaptureMouse() const { return m_capture; }

		InputMode& CentreMouse(bool centre) { m_centre = centre; return *this; }
		bool ShouldCentreMouse() const { return m_centre; }

		InputMode& OnEnter(void(*callback)()) { m_onEnter = callback; return *this; }
		void ModeEntered();

		InputMode& OnExit(void(*callback)()) { m_onExit = callback; return *this; }
		void ModeExited();

		std::string GetName() const { return m_name; }

	private:
		static InputMode s_modes[MAX_MODES];
		static int16 s_used;

		std::string m_name;
		int16 m_index { (int16) 0xFF };
		bool m_cursor { true };
		bool m_capture { false };
		bool m_centre { false };
		void (*m_onEnter)() { nullptr };
		void (*m_onExit)() { nullptr };
	};
}