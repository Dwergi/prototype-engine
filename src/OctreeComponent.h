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
	class OctreeComponent : public Component
	{
	public:
		typedef UnorderedMapPool<OctreeComponent> Pool;

		Octree::Entry Entry;

		OctreeComponent() {}

		BEGIN_TYPE( OctreeComponent )
			PARENT( Component );
		END_TYPE
	};
}