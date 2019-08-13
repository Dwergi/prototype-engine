#pragma once

#include "EnumFlags.h"
#include "InputKeys.h"

namespace dd
{
	enum class InputType : uint8
	{
		None,
		Press,
		Release,
		Hold
	};

	struct MousePosition
	{
		glm::vec2 Delta { 0, 0 };
		glm::vec2 Absolute { 0, 0 };
	};

	struct InputEvent
	{
		static const int MAX_EVENTS = 32;

		InputEvent() {}
		InputEvent(const InputEvent& other);
		InputEvent(Key k, ModifierFlags mods, InputType t) : Key(k), Modifiers(mods), Type(t) {}

		bool operator==(const InputEvent& other) const;

		Key Key { Key::NONE };
		ModifierFlags Modifiers { dd::Modifier::None };
		InputType Type { InputType::None };

		bool IsMouse() const { return Key > Key::KB_LAST; }
		bool IsKeyboard() const { return Key < Key::KB_LAST; }
	};
}