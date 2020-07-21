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
}