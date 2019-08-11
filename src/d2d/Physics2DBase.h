//
// Physics2DBase.h
// Copyright (C) Sebastian Nordgren 
// August 11th 2019
//

#pragma once

namespace d2d
{
	struct Physics2DBase
	{
		float InverseMass { 1.0f };
		float Elasticity { 1.0f };
		glm::vec2 Velocity { 0, 0 };
		int RestingFrames { 0 };

		void SetMass(float mass)
		{
			InverseMass = mass == 0 ? 0 : 1.0f / mass;
		}

		float GetMass() const
		{
			return InverseMass == 0 ? 0 : 1.0f / InverseMass;
		}

		bool IsResting() const
		{
			return RestingFrames > 10;
		}

		DD_BEGIN_CLASS(d2d::Physics2DBase)
			DD_MEMBER(InverseMass);
			DD_MEMBER(Elasticity);
			DD_MEMBER(RestingFrames);
			DD_MEMBER(Velocity);
		DD_END_CLASS()
	};
}
