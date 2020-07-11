//
// InputModeConfig.cpp
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#include "PCH.h"
#include "Input.h"

#include "DebugUI.h"
#include "IWindow.h"

#include <imgui/imgui.h>

namespace dd
{
	InputModeConfig InputModeConfig::s_modes[MAX_MODES];
	uint8 InputModeConfig::s_used = 0;

	InputModeConfig& InputModeConfig::InputModeConfig::Create(std::string_view name)
	{
		InputModeConfig* existing = Find(name);
		if (existing != nullptr)
		{
			return *existing;
		}

		DD_ASSERT(s_used < InputModeConfig::MAX_MODES);

		InputModeConfig& new_mode = s_modes[s_used];
		new_mode.m_index = s_used;
		new_mode.m_name = name;

		++s_used;

		return new_mode;
	}

	InputModeConfig* InputModeConfig::Find(std::string_view name)
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

	InputModeConfig* InputModeConfig::Access(InputModeID id)
	{
		DD_ASSERT(id == 1 || id % 2 == 0);

		uint8 index = 0;
		while (id > 1)
		{
			id = id >> 1;
			++index;
		}

		if (index > MAX_MODES || s_modes[index].m_index == -1)
		{
			return nullptr;
		}

		return &s_modes[index];
	}

	void InputModeConfig::ModeExited()
	{
		if (m_onExit != nullptr)
		{
			m_onExit();
		}
	}

	void InputModeConfig::ModeEntered()
	{
		if (m_onEnter != nullptr)
		{
			m_onEnter();
		}
	}
}