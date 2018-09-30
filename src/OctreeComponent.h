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

		DD_CLASS( dd::OctreeComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::OctreeComponent, Entry );
		}
	};
}