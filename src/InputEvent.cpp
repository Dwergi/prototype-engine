#include "PCH.h"
#include "InputEvent.h"

namespace dd
{
	InputEvent::InputEvent(const InputEvent& other) :
		Key(other.Key),
		Modifiers(other.Modifiers),
		Type(other.Type)
	{

	}

	bool InputEvent::operator==(const InputEvent& other) const
	{
		return Key == other.Key && Modifiers == other.Modifiers && Type == other.Type;
	}
}