#pragma once

#include "Component.h"
#include "Octree.h"
#include "UnorderedMapPool.h"

class OctreeComponent : public Component
{
public:
	typedef UnorderedMapPool<OctreeComponent> Pool;

	Octree::Entry Entry;

	OctreeComponent() {}
};