//
// OctreeComponent.h - Component for entities in an octree.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "IComponent.h"
#include "AABBOctree.h"
#include "DenseMapPool.h"

namespace dd
{
	class OctreeComponent : public IComponent
	{
	public:
		using Pool = DenseMapPool<OctreeComponent>;

		OctreeEntry Entry;

		OctreeComponent() {}

		BEGIN_TYPE( OctreeComponent )
			PARENT( IComponent )
			MEMBER( OctreeComponent, Entry )
		END_TYPE
	};
}