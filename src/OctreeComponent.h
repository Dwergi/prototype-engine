//
// OctreeComponent.h - Component for entities in an octree.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "AABBOctree.h"

namespace dd
{
	struct OctreeComponent
	{
	public:
		OctreeEntry Entry { -1 };

		OctreeComponent() {}

		DD_BEGIN_CLASS( dd::OctreeComponent )
			DD_COMPONENT();

			DD_MEMBER( Entry );
		DD_END_CLASS()
	};
}