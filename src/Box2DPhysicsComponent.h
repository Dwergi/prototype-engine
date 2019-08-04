#pragma once

namespace dd
{
	struct Box2DPhysicsComponent
	{
		// Hit box dimensions relative to the size of the sprite, [0-1].
		glm::vec2 HitBoxMin { 0, 0 };
		glm::vec2 HitBoxMax { 1, 1 };
		
		glm::vec2 Velocity { 0, 0 };
		float Elasticity { 0 };
		int RestingFrames { 0 };

		void Reset()
		{
			Velocity = glm::vec2(0);
			RestingFrames = 0;
		}
		bool IsResting() const
		{
			return RestingFrames >= 10;
		}

		DD_BEGIN_CLASS(dd::Box2DPhysicsComponent)
			DD_COMPONENT();

			DD_MEMBER(HitBoxMin);
			DD_MEMBER(HitBoxMax);
			DD_MEMBER(Velocity);
			DD_MEMBER(Elasticity);
			DD_MEMBER(RestingFrames);
		DD_END_CLASS()
	};
}