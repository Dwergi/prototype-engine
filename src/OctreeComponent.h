//
// OctreeComponent.h - Component for entities in an octree.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

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