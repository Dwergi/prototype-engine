//
// TreeComponent.h - Component for trees.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#pragma once

#include "Mesh.h"
#include "Random.h"

namespace dd
{
	struct TreeParticle
	{
		float Lifetime { 0 };
		float BranchChance { 0 };
		float BranchAccumulator { 0 };

		glm::vec3 Velocity { 0 };
		glm::vec3 Position { 0 };

		glm::vec3 PreviousLine { 0 };
		float LineAccumulator { 0 };
	};

	struct TreeComponent
	{
		dd::RandomFloat RNG;
		std::vector<TreeParticle> Particles;

		DD_BEGIN_CLASS( dd::TreeComponent )
			DD_COMPONENT();
		DD_END_CLASS()
	};
}