#include "PCH.h"
#include "d2d/SpriteComponent.h"

DD_TYPE_CPP(d2d::SpriteComponent);

namespace d2d
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