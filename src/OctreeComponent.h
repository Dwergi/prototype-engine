//
// OctreeComponent.h - Component for entities in an octree.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "AABBOctree.h"

namespace dd
{
	class OctreeComponent
	{
	public:
		OctreeEntry Entry;

		OctreeComponent() {}

		DD_BEGIN_TYPE( OctreeComponent )
			DD_MEMBER( OctreeComponent, Entry )
		DD_END_TYPE
	};
}