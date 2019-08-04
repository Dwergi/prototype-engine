#include "PCH.h"
#include "SpriteAnimationSystem.h"

#include "SpriteAnimationComponent.h"
#include "SpriteComponent.h"

namespace dd
{
	SpriteAnimationSystem::SpriteAnimationSystem() :
		ddc::System("Sprite Animation")
	{
		RequireTag(ddc::Tag::Visible);
		RequireWrite<dd::SpriteAnimationComponent>();
		RequireWrite<dd::SpriteComponent>();
	}

	void SpriteAnimationSystem::Update(const ddc::UpdateData& update_data)
	{
		auto anims = update_data.Data().Write<dd::SpriteAnimationComponent>();
		auto sprites = update_data.Data().Write<dd::SpriteComponent>();

		float delta_t = update_data.Delta();

		for (int i = 0; i < anims.Size(); ++i)
		{
			dd::SpriteAnimationComponent& anim = anims[i];

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