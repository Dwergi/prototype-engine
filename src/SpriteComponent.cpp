#include "PCH.h"
#include "SpriteComponent.h"

DD_TYPE_CPP(dd::SpriteComponent);

namespace dd
{
	SpriteComponent::SpriteComponent(const SpriteComponent& other) :
		Sprite(other.Sprite),
		ZIndex(other.ZIndex),
		Position(other.Position),
		Size(other.Size),
		Colour(other.Colour)
	{
	}

	SpriteComponent& SpriteComponent::operator=(const SpriteComponent& other)
	{
		Sprite = other.Sprite;
		ZIndex = other.ZIndex;
		Position = other.Position;
		Size = other.Size;
		Colour = other.Colour;

		return *this;
	}
}