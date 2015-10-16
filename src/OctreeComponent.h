//
// OctreeComponent.h - Component for entities in an octree.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Component.h"
#include "Octree.h"
#include "SortedVectorPool.h"

namespace dd
{
	class OctreeComponent : public dd::Component
	{
	public:
		typedef dd::SortedVectorPool<OctreeComponent> Pool;

		Octree::Entry Entry;

		OctreeComponent() {}

		BEGIN_MEMBERS( OctreeComponent )
		END_MEMBERS
	};
}