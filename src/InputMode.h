//
// InputMode.h
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#pragma once

#include "Flags.h"

namespace dd
{
	using InputModeID = uint16;
	using InputModeFlags = dd::Flags<InputModeID>;
	
	struct InputModeConfig
	{
		static InputModeConfig& Create(std::string name);
		static InputModeConfig* Find(std::string name);
		static InputModeConfig* Access(InputModeID id);

		InputModeID ID() const { return (1 << m_index); }

		InputModeConfig& ShowCursor(bool show_cursor) { m_cursor = show_cursor; return *this; }
		bool ShouldShowCursor() const { return m_cursor; }

		InputModeConfig& CaptureMouse(bool capture_mouse) { m_capture = capture_mouse; return *this; }
		bool ShouldCaptureMouse() const { return m_capture; }

		InputModeConfig& CentreMouse(bool centre) { m_centre = centre; return *this; }
		bool ShouldCentreMouse() const { return m_centre; }

		InputModeConfig& OnEnter(void(*callback)()) { m_onEnter = callback; return *this; }
		void ModeEntered();

		InputModeConfig& OnExit(void(*callback)()) { m_onExit = callback; return *this; }
		void ModeExited();

		std::string GetName() const { return m_name; }

	private:
		static const int MAX_MODES = sizeof(InputModeID) * 8;
		static InputModeConfig s_modes[MAX_MODES];
		static uint8 s_used;

		std::string m_name;
		uint8 m_index { 0xFF };
		bool m_cursor { true };
		bool m_capture { false };
		bool m_centre { false };
		void (*m_onEnter)() { nullptr };
		void (*m_onExit)() { nullptr };
	};
}