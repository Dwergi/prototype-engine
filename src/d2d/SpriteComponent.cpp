#include "PCH.h"
#include "d2d/SpriteComponent.h"

DD_COMPONENT_CPP(d2d::SpriteComponent);

namespace d2d
{
	SpriteComponent::SpriteComponent()
	{
		HitBox.Min = glm::vec2(0, 0);
		HitBox.Max = glm::vec2(1, 1);
	}

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