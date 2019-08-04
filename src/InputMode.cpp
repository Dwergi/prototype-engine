//
// InputMode.cpp
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#include "PCH.h"
#include "InputSystem.h"

#include "DebugUI.h"
#include "IInputSource.h"
#include "InputBindings.h"
#include "IWindow.h"

#include <imgui/imgui.h>

namespace dd
{
	InputMode InputMode::s_modes[MAX_MODES];
	int16 InputMode::s_used = 0;

	InputMode& InputMode::InputMode::Create(std::string name)
	{
		for (int i = 0; i < s_used; ++i)
		{
			DD_ASSERT(name != s_modes[s_used].m_name, "Duplicate name given for input mode: %s", name.c_str());
		}

		DD_ASSERT(s_used < InputMode::MAX_MODES);

		InputMode& new_mode = s_modes[s_used];
		new_mode.m_index = s_used;
		new_mode.m_name = name;

		++s_used;

		return new_mode;
	}

	InputMode* InputMode::Find(std::string name)
	{
		for (int i = 0; i < s_used; ++i)
		{
			if (s_modes[i].m_name == name)
			{
				return &s_modes[i];
			}
		}

		return nullptr;
	}

	InputMode* InputMode::Access(int16 id)
	{
		if (id == 0 || id >= (1 << MAX_MODES))
		{
			return nullptr;
		}

		int16 index = 0;
		while (id > 1)
		{
			id = id >> 1;
			++index;
		}

		if (index > MAX_MODES)
		{
			return nullptr;
		}

		if (s_modes[index].m_index == -1)
		{
			return nullptr;
		}

		return &s_modes[index];
	}

	void InputMode::ModeExited()
	{
		if (m_onExit != nullptr)
		{
			m_onExit();
		}
	}

	void InputMode::ModeEntered()
	{
		if (m_onEnter != nullptr)
		{
			m_onEnter();
		}
	}
}