#pragma once

#include "Component.h"

template< typename T >
class UnorderedMapPool;

class Octree::Entry;

class OctreeComponent : public Component
{
public:

	Octree::Entry Entry;

	typedef UnorderedMapPool<OctreeComponent> Pool;

	OctreeComponent()
	{

	}
};