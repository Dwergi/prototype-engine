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
		float Lifetime;
		float BranchChance;

		glm::vec3 Velocity;
		glm::vec3 Position;
	};

	struct TreeComponent
	{
		dd::RandomFloat RNG;
		std::vector<TreeParticle> Particles;

		DD_CLASS( dd::TreeComponent )
		{
			DD_COMPONENT();
		}
	};
}