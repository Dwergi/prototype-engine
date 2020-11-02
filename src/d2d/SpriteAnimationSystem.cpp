#include "PCH.h"
#include "d2d/SpriteAnimationSystem.h"

#include "d2d/SpriteAnimationComponent.h"
#include "d2d/SpriteComponent.h"

namespace d2d
{
	SpriteAnimationSystem::SpriteAnimationSystem() :
		ddc::System("Sprite Animation")
	{
		RequireTag(ddc::Tag::Visible);
		RequireWrite<d2d::SpriteAnimationComponent>();
		RequireWrite<d2d::SpriteComponent>();
	}

	void SpriteAnimationSystem::Update(ddc::UpdateData& update_data)
	{
		auto anims = update_data.Data().Write<d2d::SpriteAnimationComponent>();
		auto sprites = update_data.Data().Write<d2d::SpriteComponent>();

		float delta_t = update_data.Delta();

		for (int i = 0; i < anims.Size(); ++i)
		{
			d2d::SpriteAnimationComponent& anim = anims[i];

			if (!anim.IsPlaying)
			{
				continue;
			}

			anim.Time += delta_t;

			const float frametime = 1.0f / anim.Framerate;
			if (anim.Time > frametime)
			{
				anim.Time -= frametime;
				++anim.CurrentFrame;

				if (anim.CurrentFrame == anim.Frames.Size())
				{
					anim.CurrentFrame = 0;
				}

				sprites[i].Sprite = anim.Frames[anim.CurrentFrame];
			}
		}
	}
}