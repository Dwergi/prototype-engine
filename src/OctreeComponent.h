#pragma once

#include "Component.h"
#include "Octree.h"
#include "UnorderedMapPool.h"

namespace dd
{
	class OctreeComponent : public dd::Component
	{
	public:
		typedef dd::UnorderedMapPool<OctreeComponent> Pool;

		Octree::Entry Entry;

		OctreeComponent() {}
	};
}