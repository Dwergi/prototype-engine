//
// BoundingVolumeHierarchy.h - A bounding volume hierarchy, where objects are loosely subdivided into an octree-ish structure on the top level, 
// but no object can appear in multiple nodes.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

namespace dd
{
	class BoundingVolumeHierarchy
	{
	public:

		BoundingVolumeHierarchy();
		~BoundingVolumeHierarchy();
	};
}
