#pragma once

#include "EnumFlags.h"
#include "InputKeys.h"

namespace dd
{
	enum class InputType : uint8
	{
		None,
		Press,
		Release
	};

	struct MousePosition
	{
		glm::vec2 Delta { 0, 0 };
		glm::vec2 Absolute { 0, 0 };
	};

	struct InputEvent
	{
		InputEvent() {}
		InputEvent(const InputEvent& other);
		InputEvent(Key k, ModifierFlags mods, InputType t) : Key(k), Modifiers(mods), Type(t) {}

		bool operator==(const InputEvent& other) const;

		Key Key { Key::NONE };
		ModifierFlags Modifiers { dd::Modifier::None };
		InputType Type { InputType::None };

		bool IsMouse() const { return Key > Key::LAST_KEY; }
		bool IsKeyboard() const { return Key < Key::LAST_KEY; }
	};
}