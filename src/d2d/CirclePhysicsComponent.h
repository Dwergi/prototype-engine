#pragma once

namespace d2d
{
	struct CirclePhysicsComponent
	{
		float Radius { 0.5f };
		float Elasticity { 1 };
		glm::vec2 Velocity { 0, 0 };
		int RestingFrames { 0 };

		void Reset()
		{
			Velocity = glm::vec2(0);
			RestingFrames = 0;
		}
		bool IsResting() const { return RestingFrames >= 10; }

		DD_BEGIN_CLASS(d2d::CirclePhysicsComponent)
			DD_COMPONENT();
			DD_MEMBER(Radius); 
			DD_MEMBER(Elasticity);
			DD_MEMBER(Velocity);
			DD_MEMBER(RestingFrames);
		DD_END_CLASS()
	};
}